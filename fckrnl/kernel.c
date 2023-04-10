#include <stddef.h>
#include <stdint.h>

#include <libk/serial/log.h>
#include <libk/serial/debug.h>
#include <libk/assert.h>
#include <libk/malloc.h>
#include <libk/string.h>

#include <boot/stivale2.h>
#include <boot/boot.h>

#include <cpu/cpu.h>
#include <cpu/gdt.h>
#include <cpu/idt.h>

#include <mm/mm.h>
#include <mm/slab.h>
#include <mm/phys.h>
#include <mm/virt.h>

void kinit(struct stivale2_struct *stivale2_struct)
{
	pmm_init(stivale2_struct);
	vmm_init(stivale2_struct);

	gdt_init();
	idt_init();

	malloc_heap_init();

	log(INFO, "CPU Vendor: %s\n", cpu_get_vendor_str());

	acpi_init(stivale2_struct);
	// TODO: APIC

	struct stivale2_struct_tag_smp *smp_tag =
		stivale2_get_tag(stivale2_struct, STIVALE2_STRUCT_TAG_SMP_ID);

	asm volatile("nop");
}

void kmain(struct stivale2_struct *stivale2_struct)
{
	kinit(stivale2_struct);

	log(INFO, "All kernel parts were initialized successfully\n");

	for (;;)
		asm volatile("hlt");
}