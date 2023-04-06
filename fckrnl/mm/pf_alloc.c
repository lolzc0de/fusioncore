#include <mm/pf_alloc.h>
#include <mm/bitmap.h>

bitmap_t page_bitmap;
pf_allocator_t *allocator;

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
	allocator->free_mem = mem_size;

	uint64_t bmap_size = mem_size / 4096 / 8 + 1;

	// init bitmap
	_mm_init_bitmap(bmap_size, largest_free_mem_seg);

	// lock pages of bitmap to prevent memory corruption
	mm_lock_pages(&page_bitmap, page_bitmap.size / 4096 + 1);

	// reserve pages of unusable/reserved memory
	for (int i = 0; i < mmap_entries; i++) {
		struct EFI_MEMORY_DESCRIPTOR *desc = (struct EFI_MEMORY_DESCRIPTOR *)((uint64_t)mmap + (i * mmap_desc_size));
		if (desc->type != 7) {
			// not EfiConventionalMemory
			_mm_reserve_pages(desc->phys_addr, desc->num_pages);
		}
	}
}

void *mm_request_page()
{
	for (uint64_t index = 0; index < page_bitmap.size * 8; index++) {
		if (bitmap_get(&page_bitmap, index) == 1) continue;
		mm_lock_page((void *)(index * 4096));
		return (void *)(index * 4096);
	}

	// TODO: Page Frame Swap to File
	return NULL;
}

void mm_free_page(void *addr)
{
	uint64_t index = (uint64_t)addr / 4096;

	// is the page already free?
	if (bitmap_get(&page_bitmap, index) == 0) return;

	bitmap_set(&page_bitmap, index, 0);
	allocator->free_mem += 4096;
	allocator->used_mem -= 4096;
}

void mm_free_pages(void *addr, uint64_t page_cnt)
{
	for (int i = 0; i < page_cnt; i++) {
		mm_free_page((void *)((uint64_t)addr + (i * 4096)));
	}
}

void mm_lock_page(void *addr)
{
	uint64_t index = (uint64_t)addr / 4096;

	// is the page already locked?
	if (bitmap_get(&page_bitmap, index) == 1) return;

	bitmap_set(&page_bitmap, index, 1);
	allocator->free_mem -= 4096;
	allocator->used_mem += 4096;
}

void mm_lock_pages(void *addr, uint64_t page_cnt)
{
	for (int i = 0; i < page_cnt; i++) {
		mm_lock_page((void *)((uint64_t)addr + (i * 4096)));
	}
}

///
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

void _mm_reserve_page(void *addr)
{
	uint64_t index = (uint64_t)addr / 4096;

	// is the page already locked?
	if (bitmap_get(&page_bitmap, index) == 1) return;

	bitmap_set(&page_bitmap, index, 1);
	allocator->free_mem -= 4096;
	allocator->reserved_mem += 4096;
}

void _mm_reserve_pages(void *addr, uint64_t page_cnt)
{
	for (int i = 0; i < page_cnt; i++) {
		_mm_reserve_page((void *)((uint64_t)addr + (i * 4096)));
	}
}

void _mm_unreserve_page(void *addr)
{
	uint64_t index = (uint64_t)addr / 4096;

	// is the page already free?
	if (bitmap_get(&page_bitmap, index) == 0) return;

	bitmap_set(&page_bitmap, index, 0);
	allocator->free_mem += 4096;
	allocator->reserved_mem -= 4096;
}

void _mm_unreserve_pages(void *addr, uint64_t page_cnt)
{
	for (int i = 0; i < page_cnt; i++) {
		_mm_unreserve_page((void *)((uint64_t)addr + (i * 4096)));
	}
}

///
uint64_t mm_get_free_mem()
{
	return allocator->free_mem;
}

uint64_t mm_get_used_mem()
{
	return allocator->used_mem;
}

uint64_t mm_get_reserved_mem()
{
	return allocator->reserved_mem;
}