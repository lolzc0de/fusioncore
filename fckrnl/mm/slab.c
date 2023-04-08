#include <boot/stivale2.h>

#include <mm/slab.h>
#include <mm/phys.h>

slab_cache_t *slab_cache_create(const char *name, size_t slab_size, cache_ctor_t constructor, cache_dtor_t destructor)
{
	// assert: power of two, greater than 0, page aligned if over 4096

	slab_cache_t *cache = (slab_cache_t *)pmm_alloc(1);

	cache->name = name;
	cache->slab_size = slab_size;
	cache->constructor = constructor;
	cache->destructor = destructor;

	cache->slabs = NULL;

	return cache;
}

void slab_cache_destroy(void)
{}

void *slab_cache_alloc(slab_cache_t *cache)
{
	// TODO: Add flags
	// TODO: //
	// - Check if cache exists
	// - Check if slabs exist
	// - Iterate over all slabs, check if freelist exists
	// 		- if yes:
	// 			- remove bufctl from freelist
	// 			- return its address
	// 		- if not:
	// 			- continue
	// - grow cache if allowed
}

void slab_cache_free(slab_cache_t *cache, void *ptr)
{
	// TODO: //
	// - Check if cache exists
	// - Check if ptr exists
	// - Iterate over slabs, check if freelist existss
	// 		- if yes:
	// 			- Add ptr to freelist
}