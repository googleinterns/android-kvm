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

DEFINE_KVM_DEBUG_BUFFER(struct kvm_ubsan_info, kvm_ubsan_buffer,
                kvm_ubsan_buff_wr_ind, KVM_UBSAN_BUFFER_SIZE);

static inline struct kvm_ubsan_info *kvm_ubsan_buffer_next_slot(void)
{
	struct kvm_ubsan_info *res = NULL;
	unsigned long write_ind = __this_cpu_read(kvm_ubsan_buff_wr_ind);
	if (write_ind < KVM_UBSAN_BUFFER_SIZE) {
		res = this_cpu_ptr(&kvm_ubsan_buffer[write_ind]);
		++write_ind;
		__this_cpu_write(kvm_ubsan_buff_wr_ind, write_ind);
	}
	return res;
}

void __ubsan_handle_add_overflow(void *_data, void *lhs, void *rhs) {}

void __ubsan_handle_sub_overflow(void *_data, void *lhs, void *rhs) {}

void __ubsan_handle_mul_overflow(void *_data, void *lhs, void *rhs) {}

void __ubsan_handle_negate_overflow(void *_data, void *old_val) {}

void __ubsan_handle_divrem_overflow(void *_data, void *lhs, void *rhs) {}

void __ubsan_handle_type_mismatch(struct type_mismatch_data *data, void *ptr) {}

void __ubsan_handle_type_mismatch_v1(void *_data, void *ptr) {}

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
