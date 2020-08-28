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
#define DECLARE_KVM_DEBUG_BUFFER(type_name, buffer_name, write_ind, read_ind, laps, size)\
	DECLARE_KVM_NVHE_PER_CPU(type_name, buffer_name)[size]; 	\
	DECLARE_KVM_NVHE_PER_CPU(unsigned long, write_ind);		\
	DEFINE_PER_CPU(unsigned long, read_ind) = 0;			\
	DEFINE_PER_CPU(unsigned long, laps) = 0;
#endif

struct kvm_debug_buffer_ind {
	unsigned int read;
	unsigned int write;
	unsigned long nr_slots;
};

#ifndef __KVM_NVHE_HYPERVISOR__
/*
This is a circular buffer and it's wanted to  print the
logs in the same order as they were generated, there are two cases:
The Happy case:
	The write index didn't take a lap:
		Read from: -> read_index -> End
				-> Start -> write_index
		As long as you've got slots to read from
The Bad case:
	The write index  took at least one lap:
		Read from -> write_index + 1 -> End
			-> Start -> write_index
In both cases it is required to read from read to End
and from Start to Write.
*/
static struct kvm_debug_buffer_ind kvm_debug_buffer_get_ind(unsigned long *write_ind,
		unsigned long *read_ind, unsigned long *laps, unsigned int size)
{
	struct kvm_debug_buffer_ind res;
	unsigned int curr_lap;
	res.nr_slots =  *write_ind - *read_ind;
	if (res.nr_slots > size) {
		pr_err("The capacity is exceeded, suggested size is: %ld", res.nr_slots);
		res.nr_slots = size;
	}

	curr_lap = (*write_ind - 1) / size;
	if (curr_lap - *laps < 1) {
		res.read = *read_ind % size;
		res.write = (*write_ind - 1) % size;
	} else {
		res.read = (*write_ind + 1) % size;
		res.write = (*write_ind) % size;
	}

	*read_ind =  *write_ind;
	*laps = curr_lap;
	return res;
}
#endif
