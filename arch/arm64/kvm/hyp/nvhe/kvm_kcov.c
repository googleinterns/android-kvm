// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright 2020 Google LLC
 * Author: George Popescu <georgepope@google.com>
 */

#include <linux/preempt.h>
#include <linux/compiler.h>
#include <linux/kcov.h>
#include <linux/sched.h>
#include <asm/kvm_debug_buffer.h>
#include <asm/kvm_mmu.h>
#include <asm/kvm_hyp.h>
#include <../debug-pl011.h>

#define KVM_KCOV_BUFFER_SIZE 1000

struct kcov {
	/*
	 * Reference counter. We keep one for:
	 *  - opened file descriptor
	 *  - task with enabled coverage (we can't unwire it from another task)
	 *  - each code section for remote coverage collection
	 */
	refcount_t		refcount;
	/* The lock protects mode, size, area and t. */
	spinlock_t		lock;
	enum kcov_mode		mode;
	/* Size of arena (in long's). */
	unsigned int		size;
	/* Coverage buffer shared with user space. */
	void			*area;
	/* Task for which we collect coverage, or NULL. */
	struct task_struct	*t;
	/* Collecting coverage from remote (background) threads. */
	bool			remote;
	/* Size of remote area (in long's). */
	unsigned int		remote_size;
	/*
	 * Sequence is incremented each time kcov is reenabled, used by
	 * kcov_remote_stop(), see the comment there.
	 */
	int			sequence;
};
struct kvm_kcov_info {
        struct kcov kcov_struct;
};

DEFINE_KVM_DEBUG_BUFFER(struct kvm_kcov_info, kvm_kcvo_buffer,
                kvm_kcov_buff_wr_ind, KVM_KCOV_BUFFER_SIZE);

static bool check_kcov_mode(enum kcov_mode needed_mode, struct task_struct *t)
{
	unsigned int mode;
        /* since interrupts aren't enabled inside EL2 for the moment
		this check isn't needed 
		*/
        return 1;
	/*
	 * We are interested in code coverage as a function of a syscall inputs,
	 * so we ignore code executed in interrupts, unless we are in a remote
	 * coverage collection section in a softirq.
	
	if (!in_task() && !(in_serving_softirq() && t->kcov_softirq))
		return false;
	mode = READ_ONCE(t->kcov_mode);

	 * There is some code that runs in interrupts but for which
	 * in_interrupt() returns false (e.g. preempt_schedule_irq()).
	 * READ_ONCE()/barrier() effectively provides load-acquire wrt
	 * interrupts, there are paired barrier()/WRITE_ONCE() in
	 * kcov_start().
	
	barrier();
	return mode == needed_mode;
         */
}

static unsigned long canonicalize_ip(unsigned long ip)
{
        /* need to use the HYP->KERN va too */
#ifdef CONFIG_RANDOMIZE_BASE
	ip -= kaslr_offset();
#endif
	return ip;
}

/*
 * Entry point from instrumented code. inside EL2
 * This is called once per basic-block/edge.
 */
void  __sanitizer_cov_trace_pc(void)
{
	struct task_struct *t;
	unsigned long *area;
	//unsigned long ip = canonicalize_ip(_RET_IP_);
	unsigned long pos;
        // whenever it returns from the hvc call, the state returns to the same process
        // as it entered
        /* move this part to EL1
           write the PC to the buffer
	area = t->kcov_area;
	pos = READ_ONCE(area[0]) + 1;
	if (likely(pos < t->kcov_size)) {
		area[pos] = ip;
		WRITE_ONCE(area[0], pos);
	}
        */
}