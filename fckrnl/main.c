#include <boot/bootinfo.h>
#include <boot/efi_mem.h>
#include <mm/mm.h>
#include <mm/bitmap.h>
#include <mm/pf_alloc.h>
#include <dd/fb/fb.h>

extern uint64_t _kern_start;
extern uint64_t _kern_end;

void _start(boot_info_t *boot_info)
{
	mm_read_efi_mmap(boot_info->mmap, boot_info->mmap_size, boot_info->mmap_desc_size);

	//printf("Free Memory: %i KB\n", mm_get_free_mem(&allocator) / 1024);
	//printf("Used Memory: %i KB\n", mm_get_used_mem(&allocator) / 1024);
	//printf("Reserved Memory: %i KB\n", mm_get_reserved_mem(&allocator) / 1024);

	// reserve pages used by kernel
	uint64_t kernel_size = (uint64_t)&_kern_end - (uint64_t)&_kern_start;
	uint64_t kernel_pages = (uint64_t)&kernel_size / 4096 + 1;

	mm_lock_pages(&_kern_start, kernel_pages);
}