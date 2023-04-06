#ifndef __BOOTINFO_H_
#define __BOOTINFO_H_

#include <boot/efi_mem.h>
#include <dd/fb/fb.h>

typedef struct {
	framebuffer_t *framebuffer;
	struct EFI_MEMORY_DESCRIPTOR *mmap;
	uint64_t mmap_size;
	uint64_t mmap_desc_size;
} boot_info_t;

#endif /* __BOOTINFO_H_ */