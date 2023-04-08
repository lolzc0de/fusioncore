#include <boot/stivale2.h>
#include <libk/serial/log.h>
#include <mm/mm.h>
#include <mm/virt.h>
#include <mm/heap.h>

void heap_init(void)
{
	vmm_map_range(vmm_get_root_page_tbl(), 0, HEAP_ADDR_END,
		      HEAP_ADDR_START, KERNEL_READ_WRITE);

	log(INFO, "Heap initialized - mapped its vm region\n");
}