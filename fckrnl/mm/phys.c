#include <mm/phys.h>
#include <string.h>
#include <cpu/spinlock.h>

static uint8_t *bitmap;
uint64_t highest_addr = 0;
uint64_t last_used_index = 0;
uint64_t bitmap_size = 0;
uint64_t free_pages = 0;

lock_t pmm_lock;

void bitmap_set(size_t index)
{
	bitmap[index / 8] |= (1 << (index % 8));
}

void bitmap_clear(size_t index)
{
	bitmap[index / 8] &= ~(1 << (index % 8));
}

bool bitmap_get(size_t index)
{
	return bitmap[index / 8] & (1 << (index % 8));
}

static void *inner_alloc(uint64_t size, uint64_t limit)
{
	uint64_t p = 0;
	while (last_used_index < limit) {
		if (!bitmap_get(last_used_index++)) {
			if (++p == size) {
				uint64_t page = last_used_index - size;
				for (uint64_t i = page; i < last_used_index; i++) {
					bitmap_set(i);
				}

				return (void *)(page * PAGE_SIZE);
			}
		} else {
			p = 0;
		}
	}
	return NULL;
}

void *pmm_alloc(uint64_t size)
{
	//spinlock_lock(pmm_lock);

	uint64_t lui = last_used_index;
	void *ret = inner_alloc(size, highest_addr / PAGE_SIZE);
	if (ret == NULL) {
		last_used_index = 0;
		ret = inner_alloc(size, lui);
		if (ret == NULL) {
			// TODO: Add panic()
			// We're out of memory now
		}
	}

	free_pages -= size;

	//spinlock_unlock(pmm_lock);

	return ret;
}

void *pmm_allocz(uint64_t size)
{
	void *ret = pmm_alloc(size);
	uint64_t *ptr = (uint64_t *)(ret + hhdm_request.response->offset);

	for (uint64_t i = 0; i < (size * PAGE_SIZE) / 8; i++) {
		ptr[i] = 0;
	}

	return ret;
}

void *pmm_realloc(void *ptr, uint64_t old_size, uint64_t new_size)
{
	if (ptr == NULL)
		return pmm_alloc(new_size);
	
	if (new_size == 0) {
		pmm_free(ptr, old_size);
		return NULL;
	}

	if (new_size < old_size)
		old_size = new_size;
	
	void *realloc_ptr = pmm_alloc(new_size);
	memcpy(realloc_ptr, ptr, old_size);
	pmm_free(ptr, old_size);

	return realloc_ptr;
}

void pmm_free(void *ptr, uint64_t size)
{
	//spinlock_lock(pmm_lock);

	uint64_t page = (uint64_t)ptr / PAGE_SIZE;
	
	for (uint64_t i = page; i < page + size; i++) {
		bitmap_clear(i);
	}

	free_pages += size;
	//spinlock_unlock(pmm_lock);
}

uint64_t pmm_get_free_pages()
{
	return free_pages;
}

void pmm_init()
{
	//spinlock_lock(pmm_lock);
	struct limine_memmap_entry **mmap = memmap_request.response->entries;
	uint64_t mmap_entries = memmap_request.response->entry_count;

	for (uint64_t i = 0; i < mmap_entries; i++) {
		// skip over all unusable entries
		if (mmap[i]->type != LIMINE_MEMMAP_USABLE &&
			mmap[i]->type != LIMINE_MEMMAP_BOOTLOADER_RECLAIMABLE)
			continue;
		
		uint64_t top_addr = mmap[i]->base + mmap[i]->length;

		if (top_addr > highest_addr)
			highest_addr = top_addr;
	}

	bitmap_size = ALIGN_UP((highest_addr / PAGE_SIZE) / 8, PAGE_SIZE);

	for (uint64_t i = 0; i < mmap_entries; i++) {
		if (mmap[i]->type != LIMINE_MEMMAP_USABLE)
			continue;

		if (mmap[i]->length >= bitmap_size) {
			bitmap = (uint8_t *)(mmap[i]->base + hhdm_request.response->offset);

			memset(bitmap, 0xFF, bitmap_size);

			mmap[i]->length -= bitmap_size;
			mmap[i]->base += bitmap_size;

			break;
		}
	}

	for (uint64_t i = 0; i < mmap_entries; i++) {
		if (mmap[i]->type != LIMINE_MEMMAP_USABLE)
			continue;
		
		for (uint64_t j = 0; j < mmap[i]->length; j += PAGE_SIZE) {
			free_pages++;
			bitmap_clear((mmap[i]->base + j) / PAGE_SIZE);
		}
	}

	//spinlock_unlock(pmm_lock);
}