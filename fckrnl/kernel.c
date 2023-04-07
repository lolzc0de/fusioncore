#include <stddef.h>
#include <stdint.h>

#include <libk/serial/log.h>
#include <boot/stivale2.h>
#include <cpu/gdt.h>

void kmain(struct stivale2_struct *stivale2_struct)
{
	gdt_init();
	log(INFO, "Initialized GDT!\n");

	for (;;)
		asm volatile("hlt");
}