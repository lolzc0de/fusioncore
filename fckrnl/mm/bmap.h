#ifndef __MM_BMAP_H_
#define __MM_BMAP_H_

#include <stdint.h>
#include <stddef.h>

typedef struct {
	uint8_t *map;
	size_t size;
} bitmap_t;

void bitmap_set(bitmap_t *bitmap, int bit);
void bitmap_clear(bitmap_t *bitmap, int bit);
uint8_t bitmap_get(bitmap_t *bitmap, int bit);

#endif /* __MM_BMAP_H_ */