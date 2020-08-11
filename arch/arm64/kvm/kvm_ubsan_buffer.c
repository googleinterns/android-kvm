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

DECLARE_KVM_DEBUG_BUFFER(struct kvm_ubsan_info, kvm_ubsan_buff, KVM_UBSAN_BUFFER_SIZE);

void __kvm_check_ubsan_data(struct kvm_ubsan_info *slot)
{
	switch (slot->type) {
	case UBSAN_NONE:
		break;
	case UBSAN_OUT_OF_BOUNDS:
		__ubsan_handle_out_of_bounds(&slot->out_of_bounds_data,
				slot->u_val.lval);
		break;
	case UBSAN_UNREACHABLE_DATA:
		__ubsan_handle_builtin_unreachable(&slot->unreachable_data);
		break;
	}
}

void __kvm_check_ubsan_buffer(void)
{
	unsigned long *write_ind;
	unsigned long it;
	struct kvm_ubsan_info *slot;
	pr_err("Check UBSan Buffer\n");
	init_kvm_debug_buffer(kvm_ubsan_buff, struct kvm_ubsan_info, slot, write_ind);
	for_each_kvm_debug_buffer_slot(slot, write_ind, it) {
		__kvm_check_ubsan_data(slot);
		slot->type = 0;
	}
}

