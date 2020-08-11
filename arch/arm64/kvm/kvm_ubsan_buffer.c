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
		kvm_ubsan_buff_laps, KVM_UBSAN_BUFFER_SIZE);

void __kvm_check_ubsan_data(struct kvm_ubsan_info *slot)
{
	switch (slot->type) {
	case UBSAN_OUT_OF_BOUNDS:
		__ubsan_handle_out_of_bounds(&slot->out_of_bounds_data,
				slot->u_val.lval);
		break;
	case UBSAN_UNREACHABLE_DATA:
		__ubsan_handle_builtin_unreachable(&slot->unreachable_data);
		break;
	}
}

void iterate_kvm_ubsan_buffer(int left, int right, unsigned long *nr_slots)
{
	unsigned int i;
	struct kvm_ubsan_info *slot;

	slot = (struct kvm_ubsan_info *) this_cpu_ptr_nvhe(kvm_ubsan_buffer);
	for (i = left; i <= right && *nr_slots > 0; ++i, --*nr_slots) {
		__kvm_check_ubsan_data(slot + i);
		/* clear the slot */
		slot[i].type = 0;
	}
}

void __kvm_check_ubsan_buffer(void)
{
	struct kvm_debug_buffer_ind crt;
	unsigned long *write_ind, *read_ind, *laps;

	write_ind = (unsigned long *) this_cpu_ptr_nvhe(kvm_ubsan_buff_wr_ind);
	read_ind = this_cpu_ptr(&kvm_ubsan_buff_rd_ind);
	laps = this_cpu_ptr(&kvm_ubsan_buff_laps);
	crt = kvm_debug_buffer_get_ind(write_ind, read_ind, laps, KVM_UBSAN_BUFFER_SIZE);
	/* iterate from Right -> End */
	iterate_kvm_ubsan_buffer(crt.read, KVM_UBSAN_BUFFER_SIZE - 1,
			&crt.nr_slots);
	/* iterate from Start -> Left */
	iterate_kvm_ubsan_buffer(0, crt.write, &crt.nr_slots);
}

