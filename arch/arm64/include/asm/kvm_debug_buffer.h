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

static inline void *kvm_debug_buffer_next_slot(void *buff, unsigned long *buff_ind,
			unsigned int struct_size, unsigned long buff_size)
{
	void *res = NULL;

	if (*buff_ind < buff_size) {
		res = buff + (*buff_ind * struct_size);
		*buff_ind = *buff_ind + 1;
	}
	return res;
}

#define init_kvm_debug_buffer(buff_name, buff_type, buff_pointer, write_ind)		\
	do {										\
		buff = (buff_type *) __hyp_this_cpu_ptr(buff_name);			\
		buff_ind = (unsigned long *) __hyp_this_cpu_ptr(buff_name##_wr_ind);	\
	} while (0)

#else

#define init_kvm_debug_buffer(buff_name, buff_type, buff_pointer, write_ind)		\
	do {										\
		buff_pointer = (buff_type *) this_cpu_ptr_nvhe(buff_name);		\
		write_ind = (unsigned long *) this_cpu_ptr_nvhe(buff_name##_wr_ind);	\
	} while (0)

#define for_each_kvm_debug_buffer_slot(slot, write_ind, it)				\
	for ((it) = 0; (it) < *(write_ind); ++(it), ++(slot))

#define DECLARE_KVM_DEBUG_BUFFER(type_name, buff_name, size)				\
	DECLARE_PER_CPU(type_name, kvm_nvhe_sym(buff_name))[(size)];			\
	DECLARE_PER_CPU(unsigned long, kvm_nvhe_sym(buff_name##_wr_ind))
#endif //__KVM_NVHE_HYPERVISOR__

#else

.macro clear_kvm_debug_buffer sym tmp1, tmp2, tmp3
	mov \tmp1, 0
	hyp_str_this_cpu \sym, \tmp1, \tmp2, \tmp3
.endm

#endif // __ASSEMBLY__
