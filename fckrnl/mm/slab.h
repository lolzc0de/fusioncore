#ifndef __MM_SLAB_H_
#define __MM_SLAB_H_

#include <stdbool.h>
#include <stddef.h>

typedef struct slab_bufctl {
	struct slab_bufctl *next;
	void *ptr;
} slab_bufctl_t;

typedef struct slab {
	struct slab *next;

	size_t bufctl_count;

	slab_bufctl_t *freelist_head;
	slab_bufctl_t *freelist;
} slab_t;

typedef struct {
	const char *name;
	size_t slab_size;

	size_t bufctl_count_max;

	slab_t *slabs_head;
	slab_t *slabs;
} slab_cache_t;

slab_cache_t *slab_cache_create(const char *name, size_t slab_size);
void slab_cache_destroy(void);
void *slab_cache_alloc(slab_cache_t *cache);
void slab_cache_free(slab_cache_t *cache, void *pointer);
void slab_cache_dump(slab_cache_t *cache);

slab_bufctl_t *slab_create_bufctl(void);
void slab_create_slab(slab_cache_t *cache, slab_bufctl_t *bufctl);
void slab_init_bufctls(slab_cache_t *cache, slab_bufctl_t *bufctl, size_t index);

bool slab_cache_grow(slab_cache_t *cache, size_t count);
void slab_cache_reap(void);

///
bool is_power_of_two(int num);

#endif /* __MM_SLAB_H_ */