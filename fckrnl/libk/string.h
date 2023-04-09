#ifndef __STRING_H_
#define __STRING_H_

#include <stddef.h>
#include <stdint.h>

void *memset(void *ptr, uint32_t val, size_t n);
int memcmp(const void *p1, const void *p2, size_t n);

#endif /* __STRING_H_ */