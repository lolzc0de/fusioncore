#include <boot/stivale2.h>

#include <libk/serial/debug.h>
#include <libk/assert.h>
#include <mm/mm.h>
#include <mm/slab.h>
#include <mm/phys.h>

slab_cache_t *slab_cache_create(const char *name, size_t slab_size)
{
	assert(slab_size <= 512); // (PAGE_SIZE / 8)
	assert(is_power_of_two(slab_size));

	slab_cache_t *cache = (slab_cache_t *)pmm_alloc(1);

	if (!cache)
		return NULL;

	cache->name = name;
	cache->slab_size = slab_size;
	cache->bufctl_count_max = (PAGE_SIZE - sizeof(slab_cache_t)) / cache->slab_size;

	cache->slabs = NULL;

	slab_cache_grow(cache, 1);

	return cache;
}

void slab_cache_destroy(void)
{
}

void *slab_cache_alloc(slab_cache_t *cache)
{
	if (!cache)
		return NULL;
	
	cache->slabs = cache->slabs_head;

	for (;;) {
		if (!cache->slabs)
			return NULL;
		
		if (cache->slabs->freelist_head)
			break;
		
		cache->slabs = cache->slabs->next;
	}

	void *ptr = cache->slabs->freelist_head;

	cache->slabs->freelist_head = cache->slabs->freelist_head->next;
	cache->slabs->bufctl_count--;

	return ptr;
}

void slab_cache_free(slab_cache_t *cache, void *ptr)
{
	if (!cache)
		return;
	
	cache->slabs = cache->slabs_head;

	for (;;) {
		if (!cache->slabs)
			return;
		
		if (cache->slabs->bufctl_count < cache->bufctl_count_max)
			break;
		
		cache->slabs = cache->slabs->next;
	}

	((slab_bufctl_t *)ptr)->next = cache->slabs->freelist_head;
	cache->slabs->freelist_head = (slab_bufctl_t *)ptr;

	cache->slabs->bufctl_count++;
}

void slab_cache_dump(slab_cache_t *cache)
{
	debug("Dump for cache cache '%s'\n", cache->name);

	cache->slabs = cache->slabs_head;

	for (int slab_count = 0;; slab_count++) {
		if (!cache->slabs)
			break;

		cache->slabs->freelist = cache->slabs->freelist_head;

		debug("slab %d ptr: %p\n", slab_count, cache->slabs);
		for (int bufctl_count = 0;; bufctl_count++) {
			if (!cache->slabs->freelist)
				goto done;

			debug("\tbufctl %d ptr: %p\n", bufctl_count, cache->slabs->freelist);
			cache->slabs->freelist = cache->slabs->freelist->next;
		}

done:
		cache->slabs = cache->slabs->next;
	}
}

bool slab_cache_grow(slab_cache_t *cache, size_t count)
{
	for (size_t i = 0; i < count; i++) {
		slab_bufctl_t *bufctl = slab_create_bufctl();

		if (!bufctl)
			return false;

		slab_create_slab(cache, bufctl);

		for (size_t j = 0; j < cache->bufctl_count_max; j++) {
			slab_init_bufctls(cache, bufctl, j);
		}
	}

	return true;
}

slab_bufctl_t *slab_create_bufctl(void)
{
	slab_bufctl_t *bufctl = (slab_bufctl_t *)pmm_alloc(1);

	if (!bufctl)
		return NULL;

	bufctl->next = NULL;

	return bufctl;
}

void slab_create_slab(slab_cache_t *cache, slab_bufctl_t *bufctl)
{
	slab_t *slab =
		(slab_t *)(((uintptr_t)bufctl + PAGE_SIZE) - sizeof(slab_t));

	slab->next = NULL;
	slab->bufctl_count = cache->bufctl_count_max;
	slab->freelist_head = NULL;
	slab->freelist = NULL;

	if (!cache->slabs) {
		cache->slabs_head = slab;
		cache->slabs = slab;
	} else {
		cache->slabs->next = slab;
		cache->slabs = cache->slabs->next;
	}
}

void slab_init_bufctls(slab_cache_t *cache, slab_bufctl_t *bufctl, size_t index)
{
	slab_bufctl_t *new_bufctl = (slab_bufctl_t *)((uintptr_t)bufctl + cache->slab_size * index);
	new_bufctl->ptr = new_bufctl;

	if (!cache->slabs->freelist) {
		cache->slabs->freelist_head = new_bufctl;
		cache->slabs->freelist = new_bufctl;
	} else {
		cache->slabs->freelist->next = new_bufctl;
		cache->slabs->freelist = cache->slabs->freelist->next;
	}
}

void slab_cache_reap(void)
{
}

bool is_power_of_two(int num)
{
	return (num > 0) && ((num & (num - 1)) == 0);
}