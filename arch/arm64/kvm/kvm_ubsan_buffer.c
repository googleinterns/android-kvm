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
		KVM_UBSAN_BUFFER_SIZE);

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

void iterate_kvm_ubsan_buffer(unsigned long left, unsigned long right)
{
	unsigned long i;
	unsigned long ind;
	struct kvm_ubsan_info *slot;

	slot = (struct kvm_ubsan_info *) this_cpu_ptr_nvhe(kvm_ubsan_buffer);
	ind = left %  KVM_UBSAN_BUFFER_SIZE;
	for (i = left; i < right; ++i, ++ind) {
		if (ind >=  KVM_UBSAN_BUFFER_SIZE) {
			ind = 0;
		}
		__kvm_check_ubsan_data(slot + ind);
		slot[i].type = 0;
	}
}

void __kvm_check_ubsan_buffer(void)
{
	struct kvm_debug_buffer_ind crt;
	unsigned long *write_ind, *read_ind;

	write_ind = (unsigned long *) this_cpu_ptr_nvhe(kvm_ubsan_buff_wr_ind);
	read_ind = this_cpu_ptr(&kvm_ubsan_buff_rd_ind);
	crt = kvm_debug_buffer_get_ind(write_ind, read_ind, KVM_UBSAN_BUFFER_SIZE);
	iterate_kvm_ubsan_buffer(crt.read, crt.write);
}

