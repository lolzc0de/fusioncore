#include <libk/string.h>

void *memset(void *ptr, uint32_t val, size_t n)
{
	uint8_t *buf_ptr = (uint8_t *)ptr;

	for (size_t i = 0; i < n; i++) {
		buf_ptr[i] = val;
	}

	return ptr;
}