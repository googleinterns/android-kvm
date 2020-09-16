// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright 2020 Google LLC
 * Author: George Popescu <georgepope@google.com>
 */

#include <asm/kvm_debug_buffer.h>
#include <asm/kvm_kcov.h>

DECLARE_KVM_DEBUG_BUFFER(struct kvm_kcov_info, kvm_kcov_buff, KVM_KCOV_BUFFER_SIZE);

static bool notrace check_kcov_mode(struct kvm_kcov_info *slot, enum kcov_mode needed_mode,
		unsigned int mode)
{
	return (mode == needed_mode && slot->type == needed_mode);
}

void notrace __kvm_check_kcov_data(struct kvm_kcov_info *slot)
{
	struct task_struct *t;
	unsigned long *area;
	unsigned long pos, count, max_pos, start_index, end_pos;
	unsigned long ip = 0;

	t = current;
	area = t->kcov_area;
	ip = slot->ip;

	#ifdef CONFIG_RANDOMIZE_BASE
		ip -= kaslr_offset();
	#endif
	if (check_kcov_mode(slot, KCOV_MODE_TRACE_PC, t->kcov_mode)) {
		/* The first 64-bit word is the number of subsequent PCs. */
		pos = READ_ONCE(area[0]) + 1;
		if (likely(pos < t->kcov_size)) {
			area[pos] = ip;
			WRITE_ONCE(area[0], pos);
		}
	} else if (check_kcov_mode(slot, KCOV_MODE_TRACE_CMP, t->kcov_mode)) {
		max_pos = t->kcov_size * sizeof(unsigned long);
		count = READ_ONCE(area[0]);

		/* Every record is KCOV_WORDS_PER_CMP 64-bit words. */
		start_index = 1 + count * KCOV_WORDS_PER_CMP;
		end_pos = (start_index + KCOV_WORDS_PER_CMP) * sizeof(u64);
		if (likely(end_pos <= max_pos)) {
			area[start_index] = slot->comp_data.type;
			area[start_index + 1] = slot->comp_data.arg1;
			area[start_index + 2] = slot->comp_data.arg2;
			area[start_index + 3] = ip;
			WRITE_ONCE(area[0], count + 1);
		}
	}
}

void notrace __kvm_check_kcov_buffer(void)
{
	unsigned long *write_ind;
	unsigned long it;
	struct kvm_kcov_info *slot;

	init_kvm_debug_buffer(kvm_kcov_buff, struct kvm_kcov_info, slot, write_ind);
	for_each_kvm_debug_buffer_slot(slot, write_ind, it) {
		__kvm_check_kcov_data(slot);
		slot->type = 0;
	}
}
