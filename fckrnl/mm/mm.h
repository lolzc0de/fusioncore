#ifndef __MM_H_
#define __MM_H_

#include <stdint.h>
#include <boot/efi_mem.h>

uint64_t mm_get_size(struct EFI_MEMORY_DESCRIPTOR *mmap, uint64_t mmap_entries, uint64_t mmap_desc_size);

#endif /* __MM_H_ */