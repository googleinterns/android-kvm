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


void __kvm_check_ubsan_buffer(void)
{
	unsigned long *write_ind;
	unsigned long it;
	struct kvm_ubsan_info *slot;

	init_kvm_debug_buffer(kvm_ubsan_buff, struct kvm_ubsan_info, slot, write_ind);
	for_each_kvm_debug_buffer_slot(slot, write_ind, it) {
		/* check ubsan data */
		slot->type = 0;
	}
}

