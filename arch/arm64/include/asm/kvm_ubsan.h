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
	} type;
	union {
		struct out_of_bounds_data out_of_bounds_data;
	};
	union {
		struct ubsan_values u_val;
	};
};

void __ubsan_handle_out_of_bounds(void *_data, void *index);
