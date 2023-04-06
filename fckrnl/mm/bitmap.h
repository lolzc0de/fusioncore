#ifndef __BITMAP_H_
#define __BITMAP_H_

#include <stddef.h>
#include <stdint.h>

typedef struct {
	size_t size;
	uint8_t *buffer;
} bitmap_t;

int bitmap_get(bitmap_t *bitmap, uint64_t index);
void bitmap_set(bitmap_t *bitmap, uint64_t index, int value);

#endif /* __BITMAP_H_ */