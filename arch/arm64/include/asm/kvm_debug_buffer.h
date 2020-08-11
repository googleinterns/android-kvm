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
#include <ubsan.h>

#define NMAX 1000
#define UBSAN_MAX_TYPE 6
#define UBSAN_OO_BOUNDS 1
#define UBSAN_UNREACH_DATA 3

struct ubsan_values {
    void *lval;
    void *rval;
    char op;
};

struct kvm_debug_info {
    int type;
    union {
        struct out_of_bounds_data oo_bounds_data;
        struct shift_out_of_bounds_data soo_bounds_data;
        struct unreachable_data unreach_data;
        struct invalid_value_data invld_val_data;
        struct type_mismatch_data mism_data;
        struct overflow_data ovfw_data;
    };
    union {
        struct ubsan_values u_val;
    };
};

#ifdef __KVM_NVHE_HYPERVISOR__
    DECLARE_PER_CPU(struct kvm_debug_info, kvm_debug_buffer)[NMAX];
    DECLARE_PER_CPU(unsigned int, kvm_buff_write_ind);
#else
    DECLARE_KVM_NVHE_PER_CPU(struct kvm_debug_info, kvm_debug_buffer)[NMAX];
    DECLARE_KVM_NVHE_PER_CPU(unsigned int, kvm_buff_write_ind);
#endif   

void __ubsan_handle_out_of_bounds(void *_data, void *index);
void __ubsan_handle_builtin_unreachable(void *_data);
