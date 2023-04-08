#include <stddef.h>
#include <stdint.h>

#include <boot/stivale2.h>
#include <boot/boot.h>
#include <libk/assert.h>
#include <libk/serial/debug.h>
#include <libk/serial/log.h>
#include <mm/mm.h>
#include <mm/phys.h>
#include <mm/virt.h>
#include <cpu/gdt.h>
#include <cpu/idt.h>

void kinit(struct stivale2_struct *stivale2_struct)
{
	gdt_init();
	idt_init();
	pmm_init(stivale2_struct);
	vmm_init(stivale2_struct);
}

void kmain(struct stivale2_struct *stivale2_struct)
{
	kinit(stivale2_struct);

	asm volatile("int $0x80");

	for (;;)
		asm volatile("hlt");
}