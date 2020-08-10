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
                }
            }
        }
        /* clear the buffer */
        crt->type = 0;
    } 
    *limit = 0;   
}

