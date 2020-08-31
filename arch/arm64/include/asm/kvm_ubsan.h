/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Copyright 2020 Google LLC
 * Author: George Popescu <georgepope@google.com>
 */

#include <ubsan.h>

#define UBSAN_MAX_TYPE 6
#define KVM_UBSAN_BUFFER_SIZE 1000

struct kvm_ubsan_info {
	int type;
};
