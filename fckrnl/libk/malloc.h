#ifndef __MALLOC_H_
#define __MALLOC_H_

#include <stddef.h>

typedef struct {
	uint16_t size;
} malloc_metadata_t;

void malloc_heap_init(void);

void *malloc(size_t size);
void free(void *ptr);

///
size_t get_slab_cache_index(size_t size);

#endif /* __MALLOC_H_ */