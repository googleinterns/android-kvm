// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright 2020 Google LLC
 * Author: George Popescu <georgepope@google.com>
 */

#include <linux/ctype.h>
#include <linux/types.h>
#include <asm/kvm_debug_buffer.h>
#include <asm/kvm_arm.h>
#include <asm/kvm_asm.h>
#include <kvm/arm_pmu.h>

#include <ubsan.h>
#include <asm/kvm_ubsan.h>

DECLARE_KVM_DEBUG_BUFFER(struct kvm_ubsan_info, kvm_ubsan_buffer, 
		kvm_ubsan_buff_wr_ind, kvm_ubsan_buff_rd_ind,
		kvm_ubsan_buff_laps_did, KVM_UBSAN_BUFFER_SIZE);

void __kvm_check_ubsan_data(struct kvm_ubsan_info *slot)
{
	switch (slot->type) {
	case UBSAN_OUT_OF_BOUNDS:
		__ubsan_handle_out_of_bounds(&slot->out_of_bounds_data,
				slot->u_val.lval);
		break;
	}
}

void iterate_kvm_ubsan_buffer(int left, int right, unsigned long *nr_slots) {
	unsigned int i;
	unsigned int *limit;
	struct kvm_ubsan_info *slot;
	slot = (struct kvm_ubsan_info *) this_cpu_ptr_nvhe(kvm_ubsan_buffer);
	for (i = left; i <= right && *nr_slots > 0; ++i, --*nr_slots) {
		__kvm_check_ubsan_data(slot);
		/* clear the slot */
		slot[i].type = 0;
	}
}

void __kvm_check_ubsan_buffer(void)
{
	unsigned int ind, curr_lap, left, right;
	unsigned long *write_ind, *read_ind, *laps_did;
	unsigned long nr_slots;

	write_ind = (unsigned long *) this_cpu_ptr_nvhe(kvm_ubsan_buff_wr_ind);
	read_ind = this_cpu_ptr(&kvm_ubsan_buff_rd_ind);
	nr_slots =  *write_ind - *read_ind;
	if (nr_slots > KVM_UBSAN_BUFFER_SIZE) {
		pr_err("The capacity is exceeded, suggested size is: %ld",
				nr_slots);
		nr_slots = KVM_UBSAN_BUFFER_SIZE;
	}
	/* Because this is a circular buffer and it's wanted to  print the
	logs in the same order as they were stored, there are two cases:
	The Happy case:
		The write index didn't take a lap:
			Read from: -> read_index -> End
				   -> Start -> write_index
			As long as you've got slots to read from
	The Bad case:
		The write index  took at least one lap:
			Read from -> write_index + 1 -> End
				  -> Start -> write_index - 1
	In both cases it is required to read from one point to End
	and from Start to the same point.
	*/
	curr_lap = (nr_slots - 1) / KVM_UBSAN_BUFFER_SIZE;
	laps_did =  this_cpu_ptr(&kvm_ubsan_buff_laps_did);
	if (curr_lap - *laps_did < 1) {
		left = *read_ind % KVM_UBSAN_BUFFER_SIZE;
		right = (*write_ind - 1) % KVM_UBSAN_BUFFER_SIZE;
	} else {
		left = (*write_ind + 1) % KVM_UBSAN_BUFFER_SIZE;
		right = (*write_ind - 1) % KVM_UBSAN_BUFFER_SIZE;
	}
	ind = kvm_ubsan_buff_rd_ind % KVM_UBSAN_BUFFER_SIZE;
	/* iterate from Left -> End */
	iterate_kvm_ubsan_buffer(left, KVM_UBSAN_BUFFER_SIZE - 1, &nr_slots);
	/* iterate from Start -> Right */
	iterate_kvm_ubsan_buffer(0, right, &nr_slots);

	*read_ind =  *write_ind;
	*laps_did = curr_lap;
}

