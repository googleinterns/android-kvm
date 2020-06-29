// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (C) 2012-2015 - ARM Ltd
 * Author: Marc Zyngier <marc.zyngier@arm.com>
 */

#include <clocksource/arm_arch_timer.h>
#include <linux/compiler.h>
#include <linux/kvm_host.h>

#include <asm/kvm_hyp.h>

void __kvm_timer_set_cntvoff(u64 cntvoff)
{
	write_sysreg(cntvoff, cntvoff_el2);
}

/*
 * Should only be called on non-VHE systems.
 * VHE systems use EL2 timers and configure EL1 timers in kvm_timer_init_vhe().
 */
void __timer_restore_traps(struct kvm_vcpu *vcpu)
{
	u64 val = read_sysreg(cnthctl_el2);

	if (vcpu->arch.ctxt.is_host) {
		/* Allow physical timer/counter access for the host */
		val |= CNTHCTL_EL1PCTEN | CNTHCTL_EL1PCEN;
	} else {
		/*
		 * Disallow physical timer access for the guest
		 * Physical counter access is allowed
		 */
		val &= ~CNTHCTL_EL1PCEN;
		val |= CNTHCTL_EL1PCTEN;
	}

	write_sysreg(val, cnthctl_el2);
}
