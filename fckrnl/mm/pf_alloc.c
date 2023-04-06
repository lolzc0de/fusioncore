#include <mm/pf_alloc.h>

bitmap_t page_bitmap;

uint64_t free_mem;
uint64_t reserved_mem;
uint64_t used_mem;

void mm_read_efi_mmap(struct EFI_MEMORY_DESCRIPTOR *mmap, size_t mmap_size, size_t mmap_desc_size)
{
	uint64_t mmap_entries = mmap_size / mmap_desc_size;

	void *largest_free_mem_seg = NULL;
	size_t largest_free_mem_seg_size = 0;

	for (int i = 0; i < mmap_entries; i++) {
		struct EFI_MEMORY_DESCRIPTOR *desc = (struct EFI_MEMORY_DESCRIPTOR *)((uint64_t)mmap + (i * mmap_desc_size));
		
		if (desc->type == 7) {
			// EfiConventionalMemory
			if (desc->num_pages * 4096 > largest_free_mem_seg_size) {
				largest_free_mem_seg = desc->phys_addr;
				largest_free_mem_seg_size = desc->num_pages * 4096;
			}
		}
	}

	uint64_t mem_size = mm_get_size(mmap, mmap_entries, mmap_desc_size);
	free_mem = mem_size;

	uint64_t bmap_size = mem_size / 4096 / 8 + 1;

	// init bitmap
	_mm_init_bitmap(bmap_size, largest_free_mem_seg);

	// TODO: lock pages of bitmap
	// TODO: reserve pages of unusable/reserved memory
}

void _mm_init_bitmap(size_t bitmap_size, void *buffer_addr)
{
	page_bitmap.size = bitmap_size;
	page_bitmap.buffer = (uint8_t *)buffer_addr;

	// this could be optimized
	// however i do not give a fuck since this func
	// is going to get called only once during bootup
	for (int i = 0; i < bitmap_size; i++) {
		*(uint8_t *)(page_bitmap.buffer + i) = 0;
	}
}