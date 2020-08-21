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
#include <asm/kvm_mmu.h>
#include <asm/kvm_hyp.h>
#include <asm/kvm_kcov.h>
#include <../debug-pl011.h>

#define KVM_KCOV_BUFFER_SIZE 1000

DEFINE_KVM_DEBUG_BUFFER(struct kvm_kcov_info, kvm_kcov_buffer,
                kvm_kcov_buff_wr_ind, KVM_KCOV_BUFFER_SIZE);

static inline struct kvm_kcov_info *kvm_kcov_buffer_next_slot(void)
{
	struct kvm_kcov_info *res = NULL;
	unsigned long write_ind = __this_cpu_read(kvm_kcov_buff_wr_ind);
	unsigned long current_pos = write_ind % KVM_KCOV_BUFFER_SIZE;

	res = this_cpu_ptr(&kvm_kcov_buffer[current_pos]);
	++write_ind;
	__this_cpu_write(kvm_kcov_buff_wr_ind, write_ind);
	return res;
}

static notrace unsigned long canonicalize_ip(unsigned long ip)
{
        /* need to use the HYP->KERN va too */
	/* enable/disable RANDOMIZATION to test that*/
	//hyp_putx64(ip);
#ifdef CONFIG_RANDOMIZE_BASE
	u64 kaslr_off = kimage_vaddr - KIMAGE_VADDR;
	ip -= kaslr_off;
#endif
	return ip;
}

/*
 * Entry point from instrumented code. inside EL2
 * This is called once per basic-block/edge.
 */
#define __nvhe_undefined_symbol __kvm_hyp_vector

void notrace __sanitizer_cov_trace_pc(void)
{
	unsigned long ip = canonicalize_ip(_RET_IP_);
	struct kvm_kcov_info *slot;
	hyp_putx64(hyp_symbol_addr(__kvm_hyp_vector));
	hyp_putx64(kvm_ksym_ref(__kvm_hyp_vector));
	slot = kvm_kcov_buffer_next_slot();
	slot->ip = ip;
	slot->type = KCOV_MODE_TRACE_PC;
}

#ifdef CONFIG_KCOV_ENABLE_COMPARISONS
static void notrace write_comp_data(u64 type, u64 arg1, u64 arg2, u64 ip)
{
	struct kvm_kcov_info *slot;

	ip = canonicalize_ip(ip);
	slot = kvm_kcov_buffer_next_slot();
	/*
	 * We write all comparison arguments and types as u64.
	 * The buffer was allocated for t->kcov_size unsigned longs.
	 */
	 slot->comp_data.type = type;
	 slot->comp_data.arg1 = arg1;
	 slot->comp_data.arg2 = arg2;
	 slot->ip = ip;
	 slot->type = KCOV_MODE_TRACE_CMP;
}

void notrace __sanitizer_cov_trace_cmp1(u8 arg1, u8 arg2)
{
	write_comp_data(KCOV_CMP_SIZE(0), arg1, arg2, _RET_IP_);
}

void notrace __sanitizer_cov_trace_cmp2(u16 arg1, u16 arg2)
{
	write_comp_data(KCOV_CMP_SIZE(1), arg1, arg2, _RET_IP_);
}

void notrace __sanitizer_cov_trace_cmp4(u32 arg1, u32 arg2)
{
	write_comp_data(KCOV_CMP_SIZE(2), arg1, arg2, _RET_IP_);
}

void notrace __sanitizer_cov_trace_cmp8(u64 arg1, u64 arg2)
{
	write_comp_data(KCOV_CMP_SIZE(3), arg1, arg2, _RET_IP_);
}

void notrace __sanitizer_cov_trace_const_cmp1(u8 arg1, u8 arg2)
{
	write_comp_data(KCOV_CMP_SIZE(0) | KCOV_CMP_CONST, arg1, arg2,
			_RET_IP_);
}

void notrace __sanitizer_cov_trace_const_cmp2(u16 arg1, u16 arg2)
{
	write_comp_data(KCOV_CMP_SIZE(1) | KCOV_CMP_CONST, arg1, arg2,
			_RET_IP_);
}

void notrace __sanitizer_cov_trace_const_cmp4(u32 arg1, u32 arg2)
{
	write_comp_data(KCOV_CMP_SIZE(2) | KCOV_CMP_CONST, arg1, arg2,
			_RET_IP_);
}

void notrace __sanitizer_cov_trace_const_cmp8(u64 arg1, u64 arg2)
{
	write_comp_data(KCOV_CMP_SIZE(3) | KCOV_CMP_CONST, arg1, arg2,
			_RET_IP_);
}

void notrace __sanitizer_cov_trace_switch(u64 val, u64 *cases)
{
	u64 i;
	u64 count = cases[0];
	u64 size = cases[1];
	u64 type = KCOV_CMP_CONST;

	switch (size) {
	case 8:
		type |= KCOV_CMP_SIZE(0);
		break;
	case 16:
		type |= KCOV_CMP_SIZE(1);
		break;
	case 32:
		type |= KCOV_CMP_SIZE(2);
		break;
	case 64:
		type |= KCOV_CMP_SIZE(3);
		break;
	default:
		return;
	}
	for (i = 0; i < count; i++)
		write_comp_data(type, cases[i + 2], val, _RET_IP_);
}
#endif /* ifdef CONFIG_KCOV_ENABLE_COMPARISONS */
