/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Copyright 2020 Google LLC
 * Author: George Popescu <georgepope@google.com>
 */

#include <ubsan.h>

#define UBSAN_MAX_TYPE 6
#define KVM_UBSAN_BUFFER_SIZE 1000


struct ubsan_values {
	void *lval;
	void *rval;
	char op;
};

struct kvm_ubsan_info {
	enum {
		UBSAN_OUT_OF_BOUNDS,
		UBSAN_UNREACHABLE_DATA,
		UBSAN_SHIFT_OUT_OF_BOUNDS,
		UBSAN_INVALID_DATA,
		UBSAN_TYPE_MISMATCH,
		UBSAN_OVERFLOW_DATA
	} type;
	union {
		struct out_of_bounds_data out_of_bounds_data;
		struct unreachable_data unreachable_data;
		struct shift_out_of_bounds_data shift_out_of_bounds_data;
		struct invalid_value_data invalid_value_data;
		struct type_mismatch_data type_mismatch_data;
		struct overflow_data overflow_data;
	};
	union {
		struct ubsan_values u_val;
	};
};

void __ubsan_handle_out_of_bounds(void *_data, void *index);
void __ubsan_handle_builtin_unreachable(void *_data);
void __ubsan_handle_shift_out_of_bounds(void *_data, void *lhs, void *rhs);
void __ubsan_handle_load_invalid_value(void *_data, void *val);
void __ubsan_handle_type_mismatch(struct type_mismatch_data  *_data, void *ptr);
void __ubsan_handle_add_overflow(void *data, void *lhs, void *rhs);
void __ubsan_handle_sub_overflow(void *data, void *lhs, void *rhs);
void __ubsan_handle_mul_overflow(void *data, void *lhs, void *rhs);
void __ubsan_handle_negate_overflow(void *_data, void *old_val);
void __ubsan_handle_divrem_overflow(void *_data, void *lhs, void *rhs);
