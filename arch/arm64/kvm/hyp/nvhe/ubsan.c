// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright 2020 Google LLC
 * Author: George Popescu <georgepope@google.com>
 */
#include <linux/types.h>
#include <ubsan.h>

void __ubsan_handle_add_overflow(void *_data, void *lhs, void *rhs) {}

void __ubsan_handle_sub_overflow(void *_data, void *lhs, void *rhs) {}

void __ubsan_handle_mul_overflow(void *_data, void *lhs, void *rhs) {}

void __ubsan_handle_negate_overflow(void *_data, void *old_val) {}

void __ubsan_handle_divrem_overflow(void *_data, void *lhs, void *rhs) {}

void __ubsan_handle_type_mismatch(struct type_mismatch_data *data, void *ptr) {}

void __ubsan_handle_type_mismatch_v1(void *_data, void *ptr) {}

void __ubsan_handle_out_of_bounds(void *_data, void *index) {}

void __ubsan_handle_shift_out_of_bounds(void *_data, void *lhs, void *rhs) {}

void __ubsan_handle_builtin_unreachable(void *_data) {}

void __ubsan_handle_load_invalid_value(void *_data, void *val) {}
