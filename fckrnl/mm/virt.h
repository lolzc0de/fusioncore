#ifndef __MM_VIRT_H_
#define __MM_VIRT_H_

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>

#include <reqs.h>

#define PAGE_SIZE 4096
#define HIGHER_HALF_ADDR 0xffff800000000000
#define KERNEL_OFFS_ADDR 0xffffffff80000000

typedef struct pagemap {
	uint64_t *top_level;
} pagemap_t;

extern pagemap_t kernel_pagemap;

void vmm_init();
void vmm_switch_pagemap(pagemap_t *pagemap);
void vmm_map_page(pagemap_t *pagemap, uint64_t virtual_address, uint64_t physical_address, uint64_t flags);
pagemap_t *vmm_new_pagemap();

#endif /* __MM_VIRT_H_ */