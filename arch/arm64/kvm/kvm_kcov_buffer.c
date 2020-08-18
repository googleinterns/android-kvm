

#include <linux/kcov.h>
struct kvm_kcov_info {
        struct kcov kcov_struct;
};


DECLARE_KVM_DEBUG_BUFFER(struct kvm_kcov_info, kvm_kcov_buffer, 
		kvm_kcov_buff_wr_ind, kvm_kcov_buff_rd_ind,
		kvm_kcov_buff_laps_did, KVM_KCOV_BUFFER_SIZE);


void __kvm_check_kcov_buffer() {
        
}