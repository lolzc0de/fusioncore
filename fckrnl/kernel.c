#include <stddef.h>
#include <stdint.h>

#include <boot/stivale2.h>
#include <cpu/gdt.h>
#include <cpu/idt.h>
#include <libk/serial/log.h>

void kmain(struct stivale2_struct *stivale2_struct)
{
	gdt_init();
	idt_init();

	for (;;)
		asm volatile("hlt");
}