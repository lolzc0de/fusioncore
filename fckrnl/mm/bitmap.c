#include <mm/bitmap.h>

int bitmap_get(bitmap_t *bitmap, uint64_t index)
{
	uint64_t byte_index = index / 8;
	uint8_t bit_index = index % 8;
	uint8_t bit_indexer = 0b10000000 >> bit_index;
	if ((bitmap->buffer[byte_index] & bit_indexer) > 0)
		return 1;
	
	return 0;
}

void bitmap_set(bitmap_t *bitmap, uint64_t index, int value)
{
	uint64_t byte_index = index / 8;
	uint8_t bit_index = index % 8;
	uint8_t bit_indexer = 0b10000000 >> bit_index;

	bitmap->buffer[byte_index] &= ~bit_indexer;
	if (value) {
		bitmap->buffer[byte_index] |= bit_indexer;
	}
}