/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Standalone re-implementations of kernel interfaces for use at EL2.
 * Copyright (C) 2020 Google LLC
 * Author: Will Deacon <will@kernel.org>
 */

#ifndef __KVM_NVHE_HYPERVISOR__
#error "Attempt to include nVHE code outside of EL2 object"
#endif

#ifndef __ARM64_KVM_NVHE_UTIL_H__
#define __ARM64_KVM_NVHE_UTIL_H__

/* Locking (nvhe_spinlock_t) */
#include <nvhe/spinlock.h>

#undef spin_lock_init
#define spin_lock_init				nvhe_spin_lock_init
#undef spin_lock
#define spin_lock				nvhe_spin_lock
#undef spin_unlock
#define	spin_unlock				nvhe_spin_unlock

#endif	/* __ARM64_KVM_NVHE_UTIL_H__ */
