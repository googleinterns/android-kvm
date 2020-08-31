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
		kvm_ubsan_buff_wr_ind, KVM_UBSAN_BUFFER_SIZE);


void iterate_kvm_ubsan_buffer(unsigned long left, unsigned long right)
{
	unsigned long i;
	struct kvm_ubsan_info *slot;

	slot = (struct kvm_ubsan_info *) this_cpu_ptr_nvhe(kvm_ubsan_buffer);
	for (i = left; i < right; ++i) {
		/* check ubsan data */
		slot[i].type = 0;
	}
}

void __kvm_check_ubsan_buffer(void)
{
	unsigned long *write_ind;

	write_ind = (unsigned long *) this_cpu_ptr_nvhe(kvm_ubsan_buff_wr_ind);
	iterate_kvm_ubsan_buffer(0, *write_ind);
}

