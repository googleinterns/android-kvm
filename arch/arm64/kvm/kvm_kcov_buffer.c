// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright 2020 Google LLC
 * Author: George Popescu <georgepope@google.com>
 */

#include <asm/kvm_debug_buffer.h>
#include <asm/kvm_kcov.h>

DECLARE_KVM_DEBUG_BUFFER(struct kvm_kcov_info, kvm_kcov_buff, KVM_KCOV_BUFFER_SIZE);

void __kvm_check_kcov_buffer(void)
{
	unsigned long *write_ind;
	unsigned long it;
	struct kvm_kcov_info *slot;

	init_kvm_debug_buffer(kvm_kcov_buff, struct kvm_kcov_info, slot, write_ind);
	for_each_kvm_debug_buffer_slot(slot, write_ind, it) {
		/* check kcov data */
		slot->type = 0;
	}
}
