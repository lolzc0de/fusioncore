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

	asm volatile("int $0x80");

	for (;;)
		asm volatile("hlt");
}