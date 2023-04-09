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
}

void kmain(struct stivale2_struct *stivale2_struct)
{
	kinit(stivale2_struct);

	log(INFO, "All kernel parts were initialized successfully\n");

	for (;;)
		asm volatile("hlt");
}