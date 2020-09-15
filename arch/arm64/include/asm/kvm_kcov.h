/* SPDX-License-Identifier: GPL-2.0-only
 *
 * Copyright 2020 Google LLC
 * Author: George Popescu <georgepope@google.com>
 */

#include <linux/kcov.h>

#define KVM_KCOV_BUFFER_SIZE 1000

struct kvm_kcov_info {
	int type;
	union {
		unsigned long ip;
	};
};
