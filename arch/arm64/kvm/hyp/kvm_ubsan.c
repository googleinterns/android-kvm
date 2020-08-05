/*Copyright 2020 Google LLC

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
version 2 as published by the Free Software Foundation.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.
UBSAN error reporting functions from EL2
*/

#include <linux/bitops.h>
#include <linux/ctype.h>
#include <linux/types.h>
#include <linux/kvm_host.h>
#include <asm/kvm_arm.h>
#include <asm/kvm_mmu.h>
#include <asm/kvm_asm.h>

#ifdef CONFIG_UBSAN

#include <kvm/kvm_ubsan.h>
#include "kvm_debug_buffer.h"

extern DEFINE_PER_CPU(struct kvm_debug_info, kvm_debug_buff)[NMAX];
extern DEFINE_PER_CPU(unsigned int, kvm_buff_write_ind);

void copy_source_location_struct(struct source_location *dst, 
            struct source_location *src)
{
   dst->file_name = src->file_name;
   dst->reported = src->reported;
}

void copy_type_descriptor(struct type_descriptor **dst, 
            struct type_descriptor **src)
{
   *dst = *src;
}

void __ubsan_handle_out_of_bounds(void *_data, void *index)
{
	struct out_of_bounds_data *data = _data;
	struct kvm_debug_info *crt;
    struct out_of_bounds_data *aux_cont;
    unsigned int wr_index = __this_cpu_read(kvm_buff_write_ind);
    if (wr_index < NMAX) {
        crt = this_cpu_ptr(&kvm_debug_buff[wr_index]);
        aux_cont = &crt->oo_bounds_data;
        crt->type = UBSAN_OO_BOUNDS;
        copy_source_location_struct(&aux_cont->location, &data->location);
        copy_type_descriptor(&aux_cont->array_type, &data->array_type);
        copy_type_descriptor(&aux_cont->index_type, &data->index_type);
        crt->u_val.lval = index;
        ++wr_index;
        __this_cpu_write(kvm_buff_write_ind, wr_index);
    }
}
EXPORT_SYMBOL(__ubsan_handle_out_of_bounds);


#endif