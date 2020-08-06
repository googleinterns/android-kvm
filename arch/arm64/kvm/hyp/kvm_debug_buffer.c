/* Copyright 2020 Google LLC

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
version 2 as published by the Free Software Foundation.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.
*/


#include <linux/kvm_host.h>
#include <linux/types.h>
#include <asm/kvm_arm.h>
#include <asm/kvm_mmu.h>
#include <asm/kvm_asm.h>

#include "kvm_debug_buffer.h"

#ifdef CONFIG_UBSAN

DEFINE_PER_CPU(struct kvm_debug_info, kvm_debug_buff)[NMAX];
DEFINE_PER_CPU(unsigned int, kvm_buff_write_ind) = 0;


#endif