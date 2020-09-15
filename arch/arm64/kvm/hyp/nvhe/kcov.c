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
#include <hyp/switch.h>

/*
 * Entry point from instrumented code. inside EL2
 * This is called once per basic-block/edge.
 */

void notrace __sanitizer_cov_trace_pc(void)
{
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
