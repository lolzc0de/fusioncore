#ifndef __MM_SLAB_H_
#define __MM_SLAB_H_

#include <stddef.h>

typedef void (*cache_ctor_t)(size_t);
typedef void (*cache_dtor_t)(size_t);

typedef struct {
} slab_bufctl_t;

typedef struct {
	slab_bufctl_t *freelist;
} slab_object_t;

typedef struct {
} slab_state_t;

typedef struct {
} slab_cache_t;

#endif /* __MM_SLAB_H_ */