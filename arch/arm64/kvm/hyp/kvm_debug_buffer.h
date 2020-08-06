/* Copyright 2020 Google LLC

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
version 2 as published by the Free Software Foundation.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.
 */

#include <kvm/kvm_ubsan.h>
#include <kvm/arm_pmu.h>

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

#endif