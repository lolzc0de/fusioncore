#include <boot/stivale2.h>
#include <boot/boot.h>
#include <mm/phys.h>

void pmm_init(struct stivale2_struct *stivale2_struct)
{
	struct stivale2_struct_tag_memmap *mmap = stivale2_get_tag(
		stivale2_struct, STIVALE2_STRUCT_TAG_MEMMAP_ID);
}