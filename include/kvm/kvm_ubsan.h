/* Copyright 2020 Google LLC

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
version 2 as published by the Free Software Foundation.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.
 */
#define UBSAN_MAX_TYPE 6
#define UBSAN_OO_BOUNDS 1
#define UBSAN_SOO_BOUNDS 2
#define UBSAN_UNREACH_DATA 3
#define UBSAN_INVALID_DATA 4
#define UBSAN_MISM_DATA 5
#define UBSAN_OVFW_DATA 6

#ifndef _LIB_UBSAN_H
#define _LIB_UBSAN_H
#include <linux/types.h>

enum {
	type_kind_int = 0,
	type_kind_float = 1,
	type_unknown = 0xffff
};

struct type_descriptor {
	u16 type_kind;
	u16 type_info;
	char type_name[1];
};

struct source_location {
	const char *file_name;
	union {
		unsigned long reported;
		struct {
			u32 line;
			u32 column;
		};
	};
};

struct overflow_data {
	struct source_location location;
	struct type_descriptor *type;
};

struct type_mismatch_data {
	struct source_location location;
	struct type_descriptor *type;
	unsigned long alignment;
	unsigned char type_check_kind;
};

struct type_mismatch_data_v1 {
	struct source_location location;
	struct type_descriptor *type;
	unsigned char log_alignment;
	unsigned char type_check_kind;
};

struct type_mismatch_data_common {
	struct source_location *location;
	struct type_descriptor *type;
	unsigned long alignment;
	unsigned char type_check_kind;
};

struct nonnull_arg_data {
	struct source_location location;
	struct source_location attr_location;
	int arg_index;
};

struct out_of_bounds_data {
	struct source_location location;
	struct type_descriptor *array_type;
	struct type_descriptor *index_type;
};

struct shift_out_of_bounds_data {
	struct source_location location;
	struct type_descriptor *lhs_type;
	struct type_descriptor *rhs_type;
};

struct unreachable_data {
	struct source_location location;
};

struct invalid_value_data {
	struct source_location location;
	struct type_descriptor *type;
};

void __ubsan_handle_out_of_bounds(void *_data, void *index);
void __ubsan_handle_builtin_unreachable(void *_data);
void __ubsan_handle_shift_out_of_bounds(void *_data, void *lhs, void *rhs);

#if defined(CONFIG_ARCH_SUPPORTS_INT128)
typedef __int128 s_max;
typedef unsigned __int128 u_max;
#else
typedef s64 s_max;
typedef u64 u_max;
#endif

#endif
