#include <libk/string.h>

void *memset(void *ptr, uint32_t val, size_t n)
{
	uint8_t *buf_ptr = (uint8_t *)ptr;

	for (size_t i = 0; i < n; i++) {
		buf_ptr[i] = val;
	}

	return ptr;
}

int memcmp(const void *p1, const void *p2, size_t n)
{
	const unsigned char *ptr1 = p1;
	const unsigned char *ptr2 = p2;

	for (size_t i = 0; i < n; i++, ptr1++, ptr2++) {
		if (*ptr1 != *ptr2)
			return (*ptr1 - *ptr2);
	}

	return 0;
}