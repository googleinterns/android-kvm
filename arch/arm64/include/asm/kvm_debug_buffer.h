// SPDX-License-Identifier: GPL-2.0-only
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
#define DECLARE_KVM_DEBUG_BUFFER(type_name, buffer_name, write_ind,	\
			read_ind, laps_did, size) \
	DECLARE_KVM_NVHE_PER_CPU(type_name, buffer_name)[size]; 	\
	DECLARE_KVM_NVHE_PER_CPU(unsigned long, write_ind);		\
	DEFINE_PER_CPU(unsigned long, read_ind) = 0;			\
	DEFINE_PER_CPU(unsigned long, laps_did) = 0;
#endif   
