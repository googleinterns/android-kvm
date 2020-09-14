/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Copyright 2020 Google LLC
 * Author: George Popescu <georgepope@google.com>
 */
#ifndef __ASSEMBLY__

#include <linux/percpu-defs.h>
#include <asm/kvm_asm.h>

#ifdef __KVM_NVHE_HYPERVISOR__
#define DEFINE_KVM_DEBUG_BUFFER(type_name, buff_name, size)             \
	DEFINE_PER_CPU(type_name, buff_name)[(size)];	                \
	DEFINE_PER_CPU(unsigned long, buff_name##_wr_ind) = 0

#define DECLARE_KVM_DEBUG_BUFFER(type_name, buff_name, size)            \
	DECLARE_PER_CPU(type_name, buff_name)[(size)];                  \
	DECLARE_PER_CPU(unsigned long, buff_name##_wr_ind)

#else

#define DECLARE_KVM_DEBUG_BUFFER(type_name, buff_name, size)            \
	DECLARE_PER_CPU(type_name, kvm_nvhe_sym(buff_name))[(size)];    \
	DECLARE_PER_CPU(unsigned long, kvm_nvhe_sym(buff_name##_wr_ind))
#endif //__KVM_NVHE_HYPERVISOR__

#else

.macro clear_kvm_debug_buffer sym tmp1, tmp2, tmp3
	mov \tmp1, 0
	hyp_str_this_cpu \sym, \tmp1, \tmp2, \tmp3
.endm

#endif // __ASSEMBLY__
