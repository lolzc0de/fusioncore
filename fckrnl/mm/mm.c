#include <mm/mm.h>

uint64_t mm_get_size(struct EFI_MEMORY_DESCRIPTOR *mmap, uint64_t mmap_entries, uint64_t mmap_desc_size)
{
	static uint64_t mem_size_bytes = 0;
	if (mem_size_bytes > 0) return mem_size_bytes;

	for (int i = 0; i < mmap_entries; i++) {
		struct EFI_MEMORY_DESCRIPTOR *desc = (struct EFI_MEMORY_DESCRIPTOR *)((uint64_t)mmap + (i * mmap_desc_size));
		mem_size_bytes += desc->num_pages * 4096;
	}

	return mem_size_bytes;
}