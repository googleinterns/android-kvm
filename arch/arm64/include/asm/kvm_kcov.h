#include <linux/kcov.h>

#define KVM_KCOV_BUFFER_SIZE 1000
#define KCOV_WORDS_PER_CMP 4

struct kvm_kcov_comp_data {
        unsigned long type;
        unsigned long arg1;
        unsigned long arg2;
};

struct kvm_kcov_info {
        enum kcov_mode type;
        union {
                unsigned long ip;
                struct kvm_kcov_comp_data comp_data;
        };
};