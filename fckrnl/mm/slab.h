#ifndef __MM_SLAB_H_
#define __MM_SLAB_H_

#include <stddef.h>

#include <libk/data/linked_list.h>

typedef void (*cache_ctor_t)(size_t);
typedef void (*cache_dtor_t)(size_t);

typedef struct {
	slist_t next;
	void *ptr;
} slab_bufctl_t;

typedef struct {
	slist_t next;
	slist_t prev;

	slab_bufctl_t *freelist;
} slab_t;

typedef struct {
	const char *name;
	size_t slab_size;

	cache_ctor_t constructor;
	cache_dtor_t destructor;

	slab_t *slabs;
} slab_cache_t;

#endif /* __MM_SLAB_H_ */