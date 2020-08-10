// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright 2020 Google LLC
 * Author: George Popescu <georgepope@google.com>
 */

#include <linux/bitops.h>
#include <linux/ctype.h>
#include <linux/types.h>

#include <ubsan.h>
#include <asm/kvm_debug_buffer.h>

void write_type_mismatch_data(struct type_mismatch_data_common *data, void *lval)
{
    struct kvm_debug_info *crt;
    struct type_mismatch_data *aux_cont;
    unsigned int wr_index = __this_cpu_read(kvm_buff_write_ind);

    if (wr_index < NMAX) {
        crt = this_cpu_ptr(&kvm_debug_buffer[wr_index]);
        aux_cont = &crt->mism_data;
        crt->type = UBSAN_MISM_DATA;
        aux_cont->location.file_name = data->location->file_name;
        aux_cont->location.reported = data->location->reported;
        aux_cont->type = data->type;
        aux_cont->alignment = data->alignment;
        aux_cont->type_check_kind = data->type_check_kind;
        crt->u_val.lval = lval;
        ++wr_index;
        __this_cpu_write(kvm_buff_write_ind, wr_index);
    }
}

void __ubsan_handle_add_overflow(void *_data,
				void *lhs, void *rhs){}

void __ubsan_handle_sub_overflow(void *_data,
				void *lhs, void *rhs){}

void __ubsan_handle_mul_overflow(void *_data,
				void *lhs, void *rhs){}

void __ubsan_handle_negate_overflow(void *_data, void *old_val){}

void __ubsan_handle_divrem_overflow(void *_data, void *lhs, void *rhs){}

void __ubsan_handle_type_mismatch(struct type_mismatch_data *data,
				void *ptr)
{
	struct type_mismatch_data_common common_data = {
		.location = &data->location,
		.type = data->type,
		.alignment = data->alignment,
		.type_check_kind = data->type_check_kind
	};
	write_type_mismatch_data(&common_data, ptr);
}

void __ubsan_handle_type_mismatch_v1(void *_data, void *ptr)
{
	struct type_mismatch_data_v1 *data = _data;
	struct type_mismatch_data_common common_data = {
		.location = &data->location,
		.type = data->type,
		.alignment = 1UL << data->log_alignment,
		.type_check_kind = data->type_check_kind
	};
	write_type_mismatch_data(&common_data, ptr);
}

void __ubsan_handle_out_of_bounds(void *_data, void *index)
{
	struct kvm_debug_info *crt;
    struct out_of_bounds_data *aux_cont;
    struct out_of_bounds_data *data = _data;
    unsigned int wr_index = __this_cpu_read(kvm_buff_write_ind);
    /* if the buffer is full don't write*/
    if (wr_index < NMAX) {
        crt = this_cpu_ptr(&kvm_debug_buffer[wr_index]);
        aux_cont = &crt->oo_bounds_data;
        crt->type = UBSAN_OO_BOUNDS;
        aux_cont->location.file_name = data->location.file_name;
        aux_cont->location.reported = data->location.reported;
        aux_cont->array_type = data->array_type;
        aux_cont->index_type = data->index_type;
        crt->u_val.lval = index;
        ++wr_index;
        __this_cpu_write(kvm_buff_write_ind, wr_index);
    }
}

void __ubsan_handle_shift_out_of_bounds(void *_data, void *lhs, void *rhs)
{
    struct shift_out_of_bounds_data *data = _data;
	struct kvm_debug_info *crt;
    struct shift_out_of_bounds_data *aux_cont;
    unsigned int wr_index = __this_cpu_read(kvm_buff_write_ind);

    if (wr_index < NMAX) {
        crt = this_cpu_ptr(&kvm_debug_buffer[wr_index]);
        aux_cont = &crt->soo_bounds_data;
        crt->type = UBSAN_SOO_BOUNDS;
        aux_cont->location.file_name = data->location.file_name;
        aux_cont->location.reported = data->location.reported;
        aux_cont->lhs_type = data->lhs_type;
        aux_cont->rhs_type = data->rhs_type;
        crt->u_val.lval = lhs;
        crt->u_val.rval = rhs;
        ++wr_index;
        __this_cpu_write(kvm_buff_write_ind, wr_index);
    }
}

void __ubsan_handle_builtin_unreachable(void *_data)
{
	struct unreachable_data *data = _data;
	struct kvm_debug_info *crt;
    struct unreachable_data *aux_cont;
    unsigned int wr_index = __this_cpu_read(kvm_buff_write_ind);

    if (wr_index < NMAX) {
        crt = this_cpu_ptr(&kvm_debug_buffer[wr_index]);
        aux_cont = &crt->unreach_data;
        crt->type = UBSAN_UNREACH_DATA;
        aux_cont->location.file_name = data->location.file_name;
        aux_cont->location.reported = data->location.reported;
        ++wr_index;
        __this_cpu_write(kvm_buff_write_ind, wr_index);
    }
}

void __ubsan_handle_load_invalid_value(void *_data, void *val){
    struct invalid_value_data *data = _data;
	struct kvm_debug_info *crt;
    struct invalid_value_data *aux_cont;
    unsigned int wr_index = __this_cpu_read(kvm_buff_write_ind);

    if (wr_index < NMAX) {
        crt = this_cpu_ptr(&kvm_debug_buffer[wr_index]);
        aux_cont = &crt->invld_val_data;
        crt->type = UBSAN_INVALID_DATA;
        aux_cont->location.file_name = data->location.file_name;
        aux_cont->location.reported = data->location.reported;
        aux_cont->type = data->type;
        crt->u_val.lval = val;
        ++wr_index;
        __this_cpu_write(kvm_buff_write_ind, wr_index);
    }
}