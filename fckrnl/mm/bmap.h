#ifndef __MM_BMAP_H_
#define __MM_BMAP_H_

#include <stdint.h>

#define BIT_TO_PAGE(bit) ((size_t)bit * 0x1000)
#define PAGE_TO_BIT(page) ((size_t)page / 0x1000)

typedef struct {
	uint8_t *map;
	size_t size;
} bitmap_t;

void bitmap_set(bitmap_t *bitmap, int bit);
void bitmap_clear(bitmap_t *bitmap, int bit);
uint8_t bitmap_get(bitmap_t *bitmap, int bit);

#endif /* __MM_BMAP_H_ */