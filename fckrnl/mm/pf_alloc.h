#ifndef __PF_ALLOC_H_
#define __PF_ALLOC_H_

#include <boot/efi_mem.h>
#include <mm/mm.h>
#include <mm/bitmap.h>

#include <stdint.h>

typedef struct {
	uint64_t free_mem;
	uint64_t reserved_mem;
	uint64_t used_mem;
} pf_allocator_t;

void mm_read_efi_mmap(struct EFI_MEMORY_DESCRIPTOR *mmap, size_t mmap_size, size_t mmap_desc_size);
void *mm_request_page();
void mm_free_page(void *addr);
void mm_free_pages(void *addr, uint64_t page_cnt);
void mm_lock_page(void *addr);
void mm_lock_pages(void *addr, uint64_t page_cnt);

///
void _mm_reserve_pages(void *addr, uint64_t page_cnt);
void _mm_reserve_page(void *addr);
void _mm_unreserve_page(void *addr);
void _mm_unreserve_pages(void *addr, uint64_t page_cnt);
void _mm_init_bitmap(size_t bitmap_size, void *buffer_addr);

///
uint64_t mm_get_free_mem();
uint64_t mm_get_used_mem();
uint64_t mm_get_reserved_mem();

#endif /* __PF_ALLOC_H_ */