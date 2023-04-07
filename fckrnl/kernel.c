#include <stddef.h>
#include <stdint.h>

#include <boot/stivale2.h>
#include <cpu/gdt.h>

void kmain(struct stivale2_struct *stivale2_struct)
{
    gdt_init();

    for (;;)
        asm volatile("hlt");
}
