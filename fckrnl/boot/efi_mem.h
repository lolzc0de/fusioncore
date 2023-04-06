#ifndef __EFI_MEM_H_
#define __EFI_MEM_H_

#include <stdint.h>

struct EFI_MEMORY_DESCRIPTOR {
	uint32_t type;
	void *phys_addr;
	void *virt_addr;
	uint64_t num_pages;
	uint64_t attributes;
};

extern const char *EFI_MEMORY_TYPE_STRINGS[];

#endif // __EFI_MEM_H_