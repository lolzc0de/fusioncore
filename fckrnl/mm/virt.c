#include <mm/virt.h>
#include <mm/phys.h>

// see fckrnl/x86_64-kernel.ld.
extern char _kern_end[];

pagemap_t kernel_pagemap;

uint64_t get_kern_end_addr(void)
{
	return (uint64_t)_kern_end;
}

static uint64_t _get_next_level(uint64_t *cur_level, uint64_t entry)
{
	uint64_t *ret;

	if (cur_level[entry] & 1) {
		ret = (uint64_t *)(uint64_t)(cur_level[entry] & ~((uint64_t)0xFFF));
	} else {
		ret = pmm_allocz(1);
		cur_level[entry] = (uint64_t)ret | 0b111;
	}

	return ret;
}

pagemap_t *vmm_new_pagemap()
{
	pagemap_t *map = pmm_alloc(sizeof(pagemap_t));
	map->top_level = pmm_allocz(1);

	for (size_t i = 256; i < 512; i++)
		map->top_level[i] = kernel_pagemap.top_level[i];

	return map;
}

void vmm_map_page(pagemap_t *pagemap, uint64_t virt_addr, uint64_t phys_addr, uint64_t flags)
{
	uint64_t pagemap_entry_4 = (virt_addr & ((uint64_t)0x1ff << 39)) >> 39;
	uint64_t pagemap_entry_3 = (virt_addr & ((uint64_t)0x1ff << 20)) >> 20;
	uint64_t pagemap_entry_2 = (virt_addr & ((uint64_t)0x1ff << 21)) >> 21;
	uint64_t pagemap_entry_1 = (virt_addr & ((uint64_t)0x1ff << 12)) >> 12;

	uint64_t *pagemap_level_4 = pagemap->top_level;
	uint64_t *pagemap_level_3 = _get_next_level(pagemap_level_4, pagemap_entry_4);
	uint64_t *pagemap_level_2 = _get_next_level(pagemap_level_3, pagemap_entry_3);
	uint64_t *pagemap_level_1 = _get_next_level(pagemap_level_2, pagemap_entry_2);
	pagemap_level_1[pagemap_entry_1] = phys_addr | flags;
}

void vmm_switch_pagemap(pagemap_t *pagemap)
{
	asm volatile("mov %0, %%cr3" :: "r"(pagemap->top_level) : "memory");
}

void vmm_init()
{
	kernel_pagemap.top_level = pmm_allocz(1);

	for (uint64_t i = (uint64_t)256; i < 512; i++) {
		_get_next_level(kernel_pagemap.top_level, i);
	}

	uint64_t virt = kernel_address_request.response->virtual_base;
	uint64_t phys = kernel_address_request.response->physical_base;
	uint64_t len = ALIGN_UP(get_kern_end_addr(), PAGE_SIZE) - virt;

	for (uint64_t i = (uint64_t)0; i < len; i += PAGE_SIZE) {
		vmm_map_page(&kernel_pagemap, virt + i, phys + i, 0x03);
	}

	for (uint64_t i = (uint64_t)0x1000; i < 0x100000000; i += PAGE_SIZE) {
		vmm_map_page(&kernel_pagemap, i, i, 0x03);
		vmm_map_page(&kernel_pagemap, i + hhdm_request.response->offset, i, 0x03);
	}

	for (uint64_t i = 0; i < memmap_request.response->entry_count; i++) {
		uint64_t base = ALIGN_DOWN(memmap_request.response->entries[i]->base, PAGE_SIZE);
		uint64_t top = ALIGN_UP(memmap_request.response->entries[i]->base + memmap_request.response->entries[i]->length, PAGE_SIZE);
		if (top <= (uint64_t)0x100000000)
			continue;
		
		for (uint64_t j = base; j < top; j += PAGE_SIZE) {
			if (j < (uint64_t)0x100000000)
				continue;
			
			vmm_map_page(&kernel_pagemap, j, j, 0x03);
			vmm_map_page(&kernel_pagemap, j + hhdm_request.response->offset, j, 0x03);
		}
	}

	vmm_switch_pagemap(&kernel_pagemap);
}