// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright 2020 Google LLC
 * Author: George Popescu <georgepope@google.com>
 */
#include <linux/bitops.h>
#include <linux/ctype.h>
#include <linux/types.h>
#include <linux/percpu-defs.h>
#include <linux/kvm_host.h>
#include <asm/kvm_arm.h>
#include <asm/kvm_asm.h>
#include <asm/kvm_ubsan.h>
#include <asm/kvm_debug_buffer.h>
#include <kvm/arm_pmu.h>
#include <hyp/test_ubsan.h>


DEFINE_KVM_DEBUG_BUFFER(struct kvm_ubsan_info, kvm_ubsan_buffer,
                kvm_ubsan_buff_wr_ind, KVM_UBSAN_BUFFER_SIZE);

static inline struct kvm_ubsan_info *kvm_ubsan_buffer_next_slot(void)
{
	struct kvm_ubsan_info *res = NULL;
	unsigned long write_ind = __this_cpu_read(kvm_ubsan_buff_wr_ind);
	unsigned long current_pos = write_ind % KVM_UBSAN_BUFFER_SIZE;

	res = this_cpu_ptr(&kvm_ubsan_buffer[current_pos]);
	++write_ind;
	__this_cpu_write(kvm_ubsan_buff_wr_ind, write_ind);
	return res;
}

static void write_type_mismatch_data(struct type_mismatch_data_common *data, void *lval)
{
	struct kvm_ubsan_info *slot;
	struct type_mismatch_data *aux_cont;

	slot = kvm_ubsan_buffer_next_slot();
	if (slot) {
		slot->type = UBSAN_TYPE_MISMATCH;
		aux_cont = &slot->type_mismatch_data;
		aux_cont->location.file_name = data->location->file_name;
		aux_cont->location.reported = data->location->reported;
		aux_cont->type = data->type;
		aux_cont->alignment = data->alignment;
		aux_cont->type_check_kind = data->type_check_kind;
		slot->u_val.lval = lval;
	}
}

void write_overflow_data(struct overflow_data *data, void *lval, void *rval, char op)
{
	struct kvm_ubsan_info *slot = kvm_ubsan_buffer_next_slot();

	if (slot) {
		slot->type = UBSAN_OVERFLOW_DATA;
		slot->overflow_data = *data;
		slot->u_val.op = op;
		slot->u_val.lval = lval;
		if (op != '!')
			slot->u_val.rval = rval;
	}
}

void __ubsan_handle_add_overflow(void *_data, void *lhs, void *rhs)
{
	write_overflow_data(_data, lhs, rhs, '+');
}

void __ubsan_handle_sub_overflow(void *_data, void *lhs, void *rhs)
{
	write_overflow_data(_data, lhs, rhs, '-');
}

void __ubsan_handle_mul_overflow(void *_data, void *lhs, void *rhs)
{
	write_overflow_data(_data, lhs, rhs, '*');
}

void __ubsan_handle_negate_overflow(void *_data, void *old_val)
{
	write_overflow_data(_data, old_val, NULL, '!');
}

void __ubsan_handle_divrem_overflow(void *_data, void *lhs, void *rhs)
{
	write_overflow_data(_data, lhs, rhs, '/');
}


void __ubsan_handle_type_mismatch(struct type_mismatch_data *data, void *ptr)
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
	struct kvm_ubsan_info *slot;
	struct out_of_bounds_data *data = _data;

	slot = kvm_ubsan_buffer_next_slot();
	if (slot) {
		slot->type = UBSAN_OUT_OF_BOUNDS;
		slot->out_of_bounds_data = *data;
		slot->u_val.lval = index;
	}
}

void __ubsan_handle_shift_out_of_bounds(void *_data, void *lhs, void *rhs)
{
	struct kvm_ubsan_info *slot;
	struct shift_out_of_bounds_data *data = _data;

	slot = kvm_ubsan_buffer_next_slot();
	if (slot) {
		slot->type = UBSAN_SHIFT_OUT_OF_BOUNDS;
		slot->shift_out_of_bounds_data = *data;
		slot->u_val.lval = lhs;
		slot->u_val.rval = rhs;
	}
}

void __ubsan_handle_builtin_unreachable(void *_data)
{
	struct kvm_ubsan_info *slot;
	struct unreachable_data *data = _data;

	slot = kvm_ubsan_buffer_next_slot();
	if (slot) {
		slot->type = UBSAN_UNREACHABLE_DATA;
		slot->unreachable_data = *data;
	}
}

void __ubsan_handle_load_invalid_value(void *_data, void *val)
{
	struct kvm_ubsan_info *slot;
	struct invalid_value_data *data = _data;

	slot = kvm_ubsan_buffer_next_slot();
	if (slot) {
		slot->type = UBSAN_INVALID_DATA;
		slot->invalid_value_data = *data;
		slot->u_val.lval = val;
	}

}
