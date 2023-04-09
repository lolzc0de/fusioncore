#include <stddef.h>
#include <stdint.h>

#include <boot/stivale2.h>
#include <boot/boot.h>
#include <libk/assert.h>
#include <libk/serial/debug.h>
#include <libk/serial/log.h>
#include <mm/mm.h>
#include <mm/heap.h>
#include <mm/slab.h>
#include <mm/phys.h>
#include <mm/virt.h>
#include <cpu/gdt.h>
#include <cpu/idt.h>

void kinit(struct stivale2_struct *stivale2_struct)
{
	pmm_init(stivale2_struct);
	vmm_init(stivale2_struct);

	gdt_init();
	idt_init();

	//heap_init();
}

void kmain(struct stivale2_struct *stivale2_struct)
{
	kinit(stivale2_struct);

	log(INFO, "All kernel parts were initialized successfully\n");

	slab_cache_t *dummy1 = slab_cache_create("test cache", 512, SLAB_PANIC);

	debug("before alloc\n");
	slab_cache_dump(dummy1, SLAB_PANIC);
	debug("now allocating\n");

	void *d1ptr1 = slab_cache_alloc(dummy1, SLAB_PANIC | SLAB_AUTO_GROW);
	void *d1ptr2 = slab_cache_alloc(dummy1, SLAB_PANIC | SLAB_AUTO_GROW);
	void *d1ptr3 = slab_cache_alloc(dummy1, SLAB_PANIC | SLAB_AUTO_GROW);

	debug("d1ptr1: 0x%p\n", d1ptr1);
	debug("d1ptr2: 0x%p\n", d1ptr2);
	debug("d1ptr3: 0x%p\n", d1ptr3);

	debug("after alloc before reap\n");
	slab_cache_dump(dummy1, SLAB_PANIC);

	slab_cache_reap(dummy1, SLAB_PANIC);
	debug("after reap\n");

	slab_cache_free(dummy1, d1ptr1, SLAB_PANIC);
	slab_cache_free(dummy1, d1ptr2, SLAB_PANIC);
	slab_cache_free(dummy1, d1ptr3, SLAB_PANIC);

	debug("after free\n");
	slab_cache_dump(dummy1, SLAB_PANIC);

	slab_cache_destroy(dummy1, SLAB_PANIC);
	debug("after destroying cache\n");
	slab_cache_dump(dummy1, SLAB_PANIC);

	for (;;)
		asm volatile("hlt");
}