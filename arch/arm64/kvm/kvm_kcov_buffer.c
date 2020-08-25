

#include <asm/kvm_kcov.h>
#include <asm/kvm_debug_buffer.h>


DECLARE_KVM_DEBUG_BUFFER(struct kvm_kcov_info, kvm_kcov_buffer, 
		kvm_kcov_buff_wr_ind, kvm_kcov_buff_rd_ind,
		kvm_kcov_buff_laps, KVM_KCOV_BUFFER_SIZE);

static notrace bool check_kcov_mode(struct kvm_kcov_info *slot, enum kcov_mode needed_mode,
		struct task_struct *t)
{
	unsigned int mode;

	/*
	 * We are interested in code coverage as a function of a syscall inputs,
	 * so we ignore code executed in interrupts, unless we are in a remote
	 * coverage collection section in a softirq.
	*/
	if (!in_task() && !(in_serving_softirq() && t->kcov_softirq))
		return false;
	
	mode = READ_ONCE(t->kcov_mode);
	/*
	 * There is some code that runs in interrupts but for which
	 * in_interrupt() returns false (e.g. preempt_schedule_irq()).
	 * READ_ONCE()/barrier() effectively provides load-acquire wrt
	 * interrupts, there are paired barrier()/WRITE_ONCE() in
	 * kcov_start().
	 */
	 barrier();
	
	return (mode == needed_mode && slot->type == needed_mode);
}

void __kvm_check_kcov_data(struct kvm_kcov_info *slot) {
	struct task_struct *t;
	unsigned long *area;
	unsigned long pos;
	unsigned long count, start_index, end_pos, max_pos;

	t = current;
	area = t->kcov_area;

	#ifdef CONFIG_RANDOMIZE_BASE
	slot->ip -= kaslr_offset();
	#endif
	printk("the ip is: %lx", slot->ip);
	
	if (check_kcov_mode(slot, KCOV_MODE_TRACE_PC, t)) {
		/* The first 64-bit word is the number of subsequent PCs. */
		pos = READ_ONCE(area[0]) + 1;
		if (likely(pos < t->kcov_size)) {
			area[pos] = slot->ip;
			WRITE_ONCE(area[0], pos);
		}
	}else if (check_kcov_mode(slot, KCOV_MODE_TRACE_CMP, t)) {
		max_pos = t->kcov_size * sizeof(unsigned long);
		count = READ_ONCE(area[0]);

		/* Every record is KCOV_WORDS_PER_CMP 64-bit words. */
		start_index = 1 + count * KCOV_WORDS_PER_CMP;
		end_pos = (start_index + KCOV_WORDS_PER_CMP) * sizeof(u64);
		if (likely(end_pos <= max_pos)) {
			area[start_index] = slot->comp_data.type;
			area[start_index + 1] = slot->comp_data.arg1;
			area[start_index + 2] = slot->comp_data.arg2;
			area[start_index + 3] = slot->ip;
			WRITE_ONCE(area[0], count + 1);
		}
	}
}

void iterate_kvm_kcov_buffer(int left, int right, unsigned long *nr_slots) {
	unsigned int i;
	struct kvm_kcov_info *slot;

	slot = (struct kvm_kcov_info *) this_cpu_ptr_nvhe(kvm_kcov_buffer);
	for (i = left; i <= right && *nr_slots > 0; ++i, --*nr_slots) {
		__kvm_check_kcov_data(slot + i);
		/* clear the slot */
		slot[i].type = 0;
	}
}

void __kvm_check_kcov_buffer() {
	struct kvm_debug_buffer_ind crt;
	unsigned long *write_ind, *read_ind, *laps;

	write_ind = (unsigned long *) this_cpu_ptr_nvhe(kvm_kcov_buff_wr_ind);
	read_ind = this_cpu_ptr(&kvm_kcov_buff_rd_ind);
	laps = this_cpu_ptr(&kvm_kcov_buff_laps);
	crt = kvm_debug_buffer_get_ind(write_ind, read_ind, laps,
			KVM_KCOV_BUFFER_SIZE);
	/* iterate from Right -> End */
	iterate_kvm_kcov_buffer(crt.right, KVM_KCOV_BUFFER_SIZE - 1,
			&crt.nr_slots);
	/* iterate from Start -> Left */
	iterate_kvm_kcov_buffer(0, crt.left, &crt.nr_slots);
}