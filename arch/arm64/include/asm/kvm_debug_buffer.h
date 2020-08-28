/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Copyright 2020 Google LLC
 * Author: George Popescu <georgepope@google.com>
 */
#include <linux/percpu-defs.h>

#define KVM_DEBUG_BUFFER_SIZE 1000

#ifdef __KVM_NVHE_HYPERVISOR__
#define DEFINE_KVM_DEBUG_BUFFER(type_name, buffer_name, write_ind, size)\
	DEFINE_PER_CPU(type_name, buffer_name)[size];			\
	DEFINE_PER_CPU(unsigned long, write_ind) = 0;

#define DECLARE_KVM_DEBUG_BUFFER(type_name, buffer_name, write_ind, size)\
	DECLARE_PER_CPU(type_name, buffer_name)[size];			\
	DECLARE_PER_CPU(unsigned long, write_ind);
#else
#define DECLARE_KVM_DEBUG_BUFFER(type_name, buffer_name, write_ind, size)\
	DECLARE_KVM_NVHE_PER_CPU(type_name, buffer_name)[size]; 	\
	DECLARE_KVM_NVHE_PER_CPU(unsigned long, write_ind);
#endif //__KVM_NVHE_HYPERVISOR__

#ifdef __ASSEMBLY__
#include <asm/assembler.h>

.macro clear_buffer tmp1, tmp2, tmp3
	mov \tmp1, 0
#ifdef CONFIG_UBSAN
	str_this_cpu kvm_ubsan_buff_wr_ind, \tmp1, \tmp2, \tmp3
#endif //CONFIG_UBSAN
.endm

#endif
