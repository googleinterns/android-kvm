// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright 2020 Google LLC
 * Author: George Popescu <georgepope@google.com>
 */

#include <linux/ctype.h>
#include <linux/types.h>
#include <linux/preempt.h>
#include <linux/compiler.h>
#include <linux/kcov.h>
#include <linux/sched.h>
#include <asm/kvm_debug_buffer.h>
#include <asm/kvm_hyp.h>
#include <asm/kvm_kcov.h>
#include <hyp/switch.h>
#include <asm/kvm_kcov.h>

DEFINE_KVM_DEBUG_BUFFER(struct kvm_kcov_info, kvm_kcov_buff, KVM_KCOV_BUFFER_SIZE);

static notrace inline struct kvm_kcov_info *kvm_kcov_buffer_next_slot(void)
{
	struct kvm_kcov_info *res = NULL;
	struct kvm_kcov_info *buff;
	unsigned long *buff_ind;
	unsigned long buff_size = KVM_KCOV_BUFFER_SIZE;
	unsigned int struct_size = sizeof(struct kvm_kcov_info);

	init_kvm_debug_buffer(kvm_kcov_buff, struct kvm_kcov_info, buff, buff_ind);
	res = kvm_debug_buffer_next_slot(buff, buff_ind, struct_size, buff_size);
	return res;
}

static inline unsigned long notrace hyp_kern_va(unsigned long ip)
{
	unsigned long kern_va;
	unsigned long offset;

	asm volatile("ldr %0, =%1" : "=r" (kern_va) : "S" (__hyp_panic_string));
	offset = (unsigned long) __hyp_panic_string - kern_va;
	ip -= offset;
	return ip;
}

/*
 * Entry point from instrumented code. inside hyp/nVHE
 * This is called once per basic-block/edge.
 */

void notrace __sanitizer_cov_trace_pc(void)
{
	struct kvm_kcov_info *slot;
	unsigned long ip = hyp_kern_va(_RET_IP_);

	slot = kvm_kcov_buffer_next_slot();
	if (slot) {
		slot->ip = ip;
		slot->type = KCOV_MODE_TRACE_PC;
	}
}

#ifdef CONFIG_KCOV_ENABLE_COMPARISONS
static void notrace write_comp_data(u64 type, u64 arg1, u64 arg2, u64 ip)
{
}

void notrace __sanitizer_cov_trace_cmp1(u8 arg1, u8 arg2)
{
}

void notrace __sanitizer_cov_trace_cmp2(u16 arg1, u16 arg2)
{
}

void notrace __sanitizer_cov_trace_cmp4(u32 arg1, u32 arg2)
{
}

void notrace __sanitizer_cov_trace_cmp8(u64 arg1, u64 arg2)
{
}

void notrace __sanitizer_cov_trace_const_cmp1(u8 arg1, u8 arg2)
{
}

void notrace __sanitizer_cov_trace_const_cmp2(u16 arg1, u16 arg2)
{
}

void notrace __sanitizer_cov_trace_const_cmp4(u32 arg1, u32 arg2)
{
}

void notrace __sanitizer_cov_trace_const_cmp8(u64 arg1, u64 arg2)
{
}

void notrace __sanitizer_cov_trace_switch(u64 val, u64 *cases)
{
}
#endif /* ifdef CONFIG_KCOV_ENABLE_COMPARISONS */
