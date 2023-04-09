#include <boot/stivale2.h>
#include <libk/malloc.h>
#include <mm/slab.h>
#include <mm/phys.h>
#include <mm/mm.h>

static slab_cache_t *slab_caches[8];

void malloc_heap_init(void)
{
	slab_caches[0] = slab_cache_create("heap slab size 4", 4,
					   SLAB_PANIC | SLAB_AUTO_GROW);
	slab_caches[1] = slab_cache_create("heap slab size 8", 8,
					   SLAB_PANIC | SLAB_AUTO_GROW);
	slab_caches[2] = slab_cache_create("heap slab size 16", 16,
					   SLAB_PANIC | SLAB_AUTO_GROW);
	slab_caches[3] = slab_cache_create("heap slab size 32", 32,
					   SLAB_PANIC | SLAB_AUTO_GROW);
	slab_caches[4] = slab_cache_create("heap slab size 64", 64,
					   SLAB_PANIC | SLAB_AUTO_GROW);
	slab_caches[5] = slab_cache_create("heap slab size 128", 128,
					   SLAB_PANIC | SLAB_AUTO_GROW);
	slab_caches[6] = slab_cache_create("heap slab size 256", 256,
					   SLAB_PANIC | SLAB_AUTO_GROW);
	slab_caches[7] = slab_cache_create("heap slab size 512", 512,
					   SLAB_PANIC | SLAB_AUTO_GROW);
}

void *malloc(size_t size)
{
	void *ptr;

	if (size <= 512) {
		size_t new_size = size + sizeof(malloc_metadata_t);
		size_t index = get_slab_cache_index(new_size);

		ptr = slab_cache_alloc(slab_caches[index], SLAB_PANIC);

		malloc_metadata_t *metadata = ptr;
		metadata->size = index;
	} else {
		size_t new_size =
			ALIGN_UP(size + sizeof(malloc_metadata_t), PAGE_SIZE);
		size_t page_count = new_size / PAGE_SIZE;

		ptr = pmm_allocz(page_count);

		malloc_metadata_t *metadata = ptr;
		metadata->size = page_count;
	}

	return ptr + sizeof(malloc_metadata_t) + HEAP_START_ADDR;
}

void free(void *ptr)
{
	if (!ptr)
		return;

	ptr = ptr - HEAP_START_ADDR - sizeof(malloc_metadata_t);

	if (((uint64_t)ptr & 0xFFF) != 0) {
		malloc_metadata_t *metadata = ptr;
		size_t index = metadata->size;

		slab_cache_free(slab_caches[index], metadata, SLAB_PANIC);
	} else {
		malloc_metadata_t *metadata = ptr;
		size_t page_count = metadata->size;

		pmm_free(metadata, page_count);
	}
}

///
size_t get_slab_cache_index(size_t size)
{
	if (size <= 32) {
		if (size <= 8) {
			if (size <= 4) {
				return 0;
			} else {
				return 1;
			}
		} else {
			if (size <= 16) {
				return 2;
			} else {
				return 3;
			}
		}
	} else {
		if (size <= 128) {
			if (size <= 64) {
				return 4;
			} else {
				return 5;
			}
		} else {
			if (size <= 256) {
				return 6;
			} else {
				return 7;
			}
		}
	}
}