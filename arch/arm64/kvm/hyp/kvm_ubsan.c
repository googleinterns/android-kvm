/*Copyright 2020 Google LLC

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
version 2 as published by the Free Software Foundation.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.
UBSAN error reporting functions from EL2
*/

#include <linux/bitops.h>
#include <linux/ctype.h>
#include <linux/types.h>
#include <linux/kvm_host.h>
#include <asm/kvm_arm.h>
#include <asm/kvm_mmu.h>
#include <asm/kvm_asm.h>

#ifdef CONFIG_UBSAN

#include <kvm/kvm_ubsan.h>
#include "kvm_debug_buffer.h"

extern DEFINE_PER_CPU(struct kvm_debug_info, kvm_debug_buff)[NMAX];
extern DEFINE_PER_CPU(unsigned int, kvm_buff_write_ind);

void copy_source_location_struct(struct source_location *dst, 
            struct source_location *src)
{
   dst->file_name = src->file_name;
   dst->reported = src->reported;
}

void copy_type_descriptor(struct type_descriptor **dst, 
            struct type_descriptor **src)
{
   *dst = *src;
}

void write_type_mismatch_data(struct type_mismatch_data_common *data, void *lval)
{
    struct kvm_debug_info *crt;
    struct type_mismatch_data *aux_cont;
    unsigned int wr_index = __this_cpu_read(kvm_buff_write_ind);

    if (wr_index < NMAX) {
        crt = this_cpu_ptr(&kvm_debug_buff[wr_index]);
        aux_cont = &crt->mism_data;
        crt->type = UBSAN_MISM_DATA;
        copy_source_location_struct(&aux_cont->location, data->location);
        copy_type_descriptor(&aux_cont->type, &data->type);
        aux_cont->alignment = data->alignment;
        aux_cont->type_check_kind = data->type_check_kind;
        crt->u_val.lval = lval;
        ++wr_index;
        __this_cpu_write(kvm_buff_write_ind, wr_index);
    }
}

void __ubsan_handle_add_overflow(void *_data,
				void *lhs, void *rhs)
{
}
EXPORT_SYMBOL(__ubsan_handle_add_overflow);

void __ubsan_handle_sub_overflow(void *_data,
				void *lhs, void *rhs)
{
}
EXPORT_SYMBOL(__ubsan_handle_sub_overflow);

void __ubsan_handle_mul_overflow(void *_data,
				void *lhs, void *rhs)
{
}
EXPORT_SYMBOL(__ubsan_handle_mul_overflow);

void __ubsan_handle_negate_overflow(void *_data, void *old_val)
{
}
EXPORT_SYMBOL(__ubsan_handle_negate_overflow);

void __ubsan_handle_divrem_overflow(void *_data, void *lhs, void *rhs)
{
}
EXPORT_SYMBOL(__ubsan_handle_divrem_overflow);

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
EXPORT_SYMBOL(__ubsan_handle_type_mismatch);

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
EXPORT_SYMBOL(__ubsan_handle_type_mismatch_v1);

void __ubsan_handle_out_of_bounds(void *_data, void *index)
{
	struct out_of_bounds_data *data = _data;
	struct kvm_debug_info *crt;
    struct out_of_bounds_data *aux_cont;
    unsigned int wr_index = __this_cpu_read(kvm_buff_write_ind);

    if (wr_index < NMAX) {
        crt = this_cpu_ptr(&kvm_debug_buff[wr_index]);
        aux_cont = &crt->oo_bounds_data;
        crt->type = UBSAN_OO_BOUNDS;
        copy_source_location_struct(&aux_cont->location, &data->location);
        copy_type_descriptor(&aux_cont->array_type, &data->array_type);
        copy_type_descriptor(&aux_cont->index_type, &data->index_type);
        crt->u_val.lval = index;
        ++wr_index;
        __this_cpu_write(kvm_buff_write_ind, wr_index);
    }
}
EXPORT_SYMBOL(__ubsan_handle_out_of_bounds);

void __ubsan_handle_shift_out_of_bounds(void *_data, void *lhs, void *rhs)
{
	struct shift_out_of_bounds_data *data = _data;
	struct kvm_debug_info *crt;
    struct shift_out_of_bounds_data *aux_cont;
    unsigned int wr_index = __this_cpu_read(kvm_buff_write_ind);

    if (wr_index < NMAX) {
        crt = this_cpu_ptr(&kvm_debug_buff[wr_index]);
        aux_cont = &crt->soo_bounds_data;
        crt->type = UBSAN_SOO_BOUNDS;
        copy_source_location_struct(&aux_cont->location, &data->location);
        copy_type_descriptor(&aux_cont->lhs_type, &data->lhs_type);
        copy_type_descriptor(&aux_cont->rhs_type, &data->rhs_type);
        crt->u_val.lval = lhs;
        crt->u_val.rval = rhs;
        ++wr_index;
        __this_cpu_write(kvm_buff_write_ind, wr_index);
    }
}
EXPORT_SYMBOL(__ubsan_handle_shift_out_of_bounds);


void __ubsan_handle_builtin_unreachable(void *_data)
{
	struct unreachable_data *data = _data;
	struct kvm_debug_info *crt;
    struct unreachable_data *aux_cont;
    unsigned int wr_index = __this_cpu_read(kvm_buff_write_ind);

    if (wr_index < NMAX) {
        crt = this_cpu_ptr(&kvm_debug_buff[wr_index]);
        aux_cont = &crt->unreach_data;
        crt->type = UBSAN_UNREACH_DATA;
        copy_source_location_struct(&aux_cont->location, &data->location);
        ++wr_index;
        __this_cpu_write(kvm_buff_write_ind, wr_index);
    }
}
EXPORT_SYMBOL(__ubsan_handle_builtin_unreachable);

void __ubsan_handle_load_invalid_value(void *_data, void *val)
{
	struct invalid_value_data *data = _data;
	struct kvm_debug_info *crt;
    struct invalid_value_data *aux_cont;
    unsigned int wr_index = __this_cpu_read(kvm_buff_write_ind);

    if (wr_index < NMAX) {
        crt = this_cpu_ptr(&kvm_debug_buff[wr_index]);
        aux_cont = &crt->invld_val_data;
        crt->type = UBSAN_INVALID_DATA;
        copy_source_location_struct(&aux_cont->location, &data->location);
        copy_type_descriptor(&aux_cont->type, &data->type);
        crt->u_val.lval = val;
        ++wr_index;
        __this_cpu_write(kvm_buff_write_ind, wr_index);
    }
}
EXPORT_SYMBOL(__ubsan_handle_load_invalid_value);
#endif