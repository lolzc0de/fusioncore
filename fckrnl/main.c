#include <boot/bootinfo.h>
#include <boot/efi_mem.h>
#include <dd/fb/fb.h>

void _start(boot_info_t *boot_info)
{
	uint64_t mmap_entries = boot_info->mmap_size / boot_info->mmap_desc_size;

	for (int i = 0; i < mmap_entries; i++) {
		struct EFI_MEMORY_DESCRIPTOR* desc = (struct EFI_MEMORY_DESCRIPTOR*)((uint64_t)boot_info->mmap + (i * boot_info->mmap_desc_size));
	}
}