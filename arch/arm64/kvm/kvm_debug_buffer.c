// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright 2020 Google LLC
 * Author: George Popescu <georgepope@google.com>
 */

#include <linux/ctype.h>
#include <linux/types.h>
#include <ubsan.h>
#include <asm/kvm_debug_buffer.h>
#include <asm/kvm_arm.h>
#include <asm/kvm_asm.h>
#include <kvm/arm_pmu.h>

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
                __ubsan_handle_type_mismatch(&crt->mism_data, crt->u_val.lval);
                break;
            case UBSAN_OVFW_DATA:
                if (crt->u_val.op == '/') {
                    __ubsan_handle_divrem_overflow(&crt->ovfw_data, crt->u_val.lval, crt->u_val.rval);
                } else if (crt->u_val.op == '!') {
                    __ubsan_handle_negate_overflow(&crt->ovfw_data, crt->u_val.lval);
                } else if (crt->u_val.op == '+'){
                    __ubsan_handle_add_overflow(&crt->ovfw_data, crt->u_val.lval, crt->u_val.rval);
                } else if (crt->u_val.op == '-') {
                    __ubsan_handle_sub_overflow(&crt->ovfw_data, crt->u_val.lval, crt->u_val.rval);
                } else if (crt->u_val.op == '*') {
                    __ubsan_handle_mul_overflow(&crt->ovfw_data, crt->u_val.lval, crt->u_val.rval);
                }
                break;
    }
}
#endif

void __kvm_check_buffer(void)
{
    unsigned int i;
    unsigned int *limit;
    struct kvm_debug_info *crt;

    crt = (struct kvm_debug_info *) this_cpu_ptr_nvhe(kvm_debug_buffer);
    limit = this_cpu_ptr_nvhe(kvm_buff_write_ind);
    for (i = 0; i < *limit; i++, crt++) {
        if (crt->type != 0) {
            if (crt->type <= UBSAN_MAX_TYPE) {
                if (!IS_ENABLED(CONFIG_UBSAN_TRAP)) {
                    /* check what is returned by UBSAN */
                    __kvm_check_ubsan_data(crt); 
                }
            }
        }
        /* clear the buffer */
        crt->type = 0;
    } 
    *limit = 0;   
}

