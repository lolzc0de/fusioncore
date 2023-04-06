#ifndef __PF_ALLOC_H_
#define __PF_ALLOC_H_

#include <boot/efi_mem.h>
#include <mm/mm.h>
#include <mm/bitmap.h>

#include <stdint.h>

void mm_read_efi_mmap(struct EFI_MEMORY_DESCRIPTOR *mmap, size_t mmap_size, size_t mmap_desc_size);

void _mm_init_bitmap(size_t bitmap_size, void *buffer_addr);

#endif /* __PF_ALLOC_H_ */