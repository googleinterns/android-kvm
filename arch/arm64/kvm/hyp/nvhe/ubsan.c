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

DEFINE_KVM_DEBUG_BUFFER(struct kvm_ubsan_info, kvm_ubsan_buff, KVM_UBSAN_BUFFER_SIZE);

static inline struct kvm_ubsan_info *kvm_ubsan_buffer_next_slot(void)
{
	struct kvm_ubsan_info *res;
	struct kvm_ubsan_info *buff;
	unsigned long *buff_ind;
	unsigned long buff_size = KVM_UBSAN_BUFFER_SIZE;
	unsigned int struct_size = sizeof(struct kvm_ubsan_info);

	init_kvm_debug_buffer(kvm_ubsan_buff, struct kvm_ubsan_info, buff, buff_ind);
	res = kvm_debug_buffer_next_slot(buff, buff_ind, struct_size, buff_size);
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
	struct kvm_ubsan_info *slot = NULL;
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

void __ubsan_handle_load_invalid_value(void *_data, void *val) {}
