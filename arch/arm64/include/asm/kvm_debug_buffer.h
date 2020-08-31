/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Copyright 2020 Google LLC
 * Author: George Popescu <georgepope@google.com>
 */

#include <linux/percpu-defs.h>
#include <asm/kvm_arm.h>
#include <asm/kvm_asm.h>
#include <linux/kvm_host.h>
#include <kvm/arm_pmu.h>

#define KVM_DEBUG_BUFFER_SIZE 1000

#ifdef __KVM_NVHE_HYPERVISOR__
#define DEFINE_KVM_DEBUG_BUFFER(type_name, buffer_name, write_ind, size)\
	DEFINE_PER_CPU(type_name, buffer_name)[size];			\
	DEFINE_PER_CPU(unsigned long, write_ind) = 0;

#define DECLARE_KVM_DEBUG_BUFFER(type_name, buffer_name, write_ind, size)\
	DECLARE_PER_CPU(type_name, buffer_name)[size];			\
	DECLARE_PER_CPU(unsigned long, write_ind);
#else
#define DECLARE_KVM_DEBUG_BUFFER(type_name, buffer_name, write_ind, read_ind, size)\
	DECLARE_KVM_NVHE_PER_CPU(type_name, buffer_name)[size]; 	\
	DECLARE_KVM_NVHE_PER_CPU(unsigned long, write_ind);		\
	DEFINE_PER_CPU(unsigned long, read_ind) = 0;
#endif

struct kvm_debug_buffer_ind {
	unsigned int read;
	unsigned int write;
};

#ifndef __KVM_NVHE_HYPERVISOR__
/*
This is a circular buffer and it's wanted to  print the
logs in the same order as they were generated.
*/
static struct kvm_debug_buffer_ind kvm_debug_buffer_get_ind(unsigned long *write_ind,
		unsigned long *read_ind, unsigned int size)
{
	struct kvm_debug_buffer_ind res;
	
	res.write = *write_ind;
	/* compute the read index */
	if (*write_ind > size) {
		if (*write_ind - *read_ind > size)
			res.read = *write_ind - size;
		else res.read = *read_ind;
	}
	*read_ind =  *write_ind;
	return res;
}
#endif
