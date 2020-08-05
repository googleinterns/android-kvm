/*
Copyright 2020 Google LLC

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
version 2 as published by the Free Software Foundation.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.
*/

#include "linux/percpu-defs.h"
#include <kvm/kvm_ubsan.h>
#include <asm/kvm_arm.h>
#include <asm/kvm_asm.h>

#ifdef CONFIG_UBSAN

#define NMAX 1000

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

DECLARE_KVM_NVHE_PER_CPU(struct kvm_debug_info, kvm_debug_buff)[NMAX];
DECLARE_KVM_NVHE_PER_CPU(unsigned int, kvm_buff_write_ind);

#ifndef CONFIG_UBSAN_TRAP
void __kvm_check_ubsan_data(struct kvm_debug_info *crt)
{
    switch (crt->type) {
            case UBSAN_OO_BOUNDS:
                __ubsan_handle_out_of_bounds(&crt->oo_bounds_data, crt->u_val.lval);
                break;
            case UBSAN_SOO_BOUNDS:
                __ubsan_handle_shift_out_of_bounds(&crt->soo_bounds_data, crt->u_val.lval, 
                                    crt->u_val.rval);
                break;
            case UBSAN_UNREACH_DATA:
                __ubsan_handle_builtin_unreachable(&crt->unreach_data);
                break;
            case UBSAN_INVALID_DATA:
                __ubsan_handle_load_invalid_value(&crt->invld_val_data, crt->u_val.lval);
                break;
            case UBSAN_MISM_DATA:
                break;
            case UBSAN_OVFW_DATA:
                break;
            default:
                break;
        }
}
#endif

void __kvm_check_buffer(void)
{
    unsigned int i;
    unsigned int *limit;
    struct kvm_debug_info *crt;
    crt = (struct kvm_debug_info *) this_cpu_ptr_nvhe(kvm_debug_buff);
    limit = this_cpu_ptr_nvhe(kvm_buff_write_ind);
    for (i = 0; i < *limit; i++, crt++) {
        if (crt->type != 0) {
            if (crt->type <= UBSAN_MAX_TYPE) {
                #ifndef CONFIG_UBSAN_TRAP
                    __kvm_check_ubsan_data(crt);
                #endif
            }
        }
        /* clear the buffer*/
        crt->type = 0;
    } 
    *limit = 0;   
}
EXPORT_SYMBOL(__kvm_check_buffer);

#endif