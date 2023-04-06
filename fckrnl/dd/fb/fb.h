#ifndef __FB_H_
#define __FB_H_

#include <stddef.h>

typedef struct {
	void *base_addr;
	size_t buf_size;
	unsigned int width;
	unsigned int height;
	unsigned int pixels_per_scanline;
} framebuffer_t;

#endif /* __FB_H_ */