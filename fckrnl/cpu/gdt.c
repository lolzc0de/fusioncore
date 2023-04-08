#include <cpu/gdt.h>
#include <libk/string.h>

extern uint8_t stack[16384];

static tss_t tss;
static gdt_t gdt;
static gdt_ptr_t gdt_ptr;

extern void _load_gdt_tss(uint64_t gdt_ptr);

void gdt_init(void)
{
	// segment 0x00 - null descriptor
	gdt.entries[GDT_NULL_DESCRIPTOR].limit_lo = 0;
	gdt.entries[GDT_NULL_DESCRIPTOR].base_lo = 0;
	gdt.entries[GDT_NULL_DESCRIPTOR].base_mid = 0;
	gdt.entries[GDT_NULL_DESCRIPTOR].access = 0;
	gdt.entries[GDT_NULL_DESCRIPTOR].flags = 0;
	gdt.entries[GDT_NULL_DESCRIPTOR].base_hi = 0;

	// segment 0x08 - kernel code
	gdt.entries[GDT_KERNEL_CODE].limit_lo = 0;
	gdt.entries[GDT_KERNEL_CODE].base_lo = 0;
	gdt.entries[GDT_KERNEL_CODE].base_mid = 0;
	gdt.entries[GDT_KERNEL_CODE].access = 0b10011010;
	gdt.entries[GDT_KERNEL_CODE].flags = 0b00100000;
	gdt.entries[GDT_KERNEL_CODE].base_hi = 0;

	// segment 0x10 - kernel data
	gdt.entries[GDT_KERNEL_DATA].limit_lo = 0;
	gdt.entries[GDT_KERNEL_DATA].base_lo = 0;
	gdt.entries[GDT_KERNEL_DATA].base_mid = 0;
	gdt.entries[GDT_KERNEL_DATA].access = 0b10010010;
	gdt.entries[GDT_KERNEL_DATA].flags = 0;
	gdt.entries[GDT_KERNEL_DATA].base_hi = 0;

	// segment 0x18 - user data
	gdt.entries[GDT_USER_DATA].limit_lo = 0;
	gdt.entries[GDT_USER_DATA].base_lo = 0;
	gdt.entries[GDT_USER_DATA].base_mid = 0;
	gdt.entries[GDT_USER_DATA].access = 0b11110010;
	gdt.entries[GDT_USER_DATA].flags = 0;
	gdt.entries[GDT_USER_DATA].base_hi = 0;

	// segment 0x20 - user code
	gdt.entries[GDT_USER_CODE].limit_lo = 0;
	gdt.entries[GDT_USER_CODE].base_lo = 0;
	gdt.entries[GDT_USER_CODE].base_mid = 0;
	gdt.entries[GDT_USER_CODE].access = 0b11111010;
	gdt.entries[GDT_USER_CODE].flags = 0b00100000;
	gdt.entries[GDT_USER_CODE].base_hi = 0;

	// segment 0x28 - tss entry
	gdt.tss_descriptor.length = 104;
	gdt.tss_descriptor.base_lo = 0;
	gdt.tss_descriptor.base_mid = 0;
	gdt.tss_descriptor.flags1 = 0b10001001;
	gdt.tss_descriptor.flags2 = 0;
	gdt.tss_descriptor.base_hi = 0;
	gdt.tss_descriptor.base_upper = 0;
	gdt.tss_descriptor.reserved = 0;

	memset(&tss, 0, sizeof(tss));

	tss.rsp[0] = (uintptr_t)stack + sizeof(stack);
	tss.iopb_offset = sizeof(tss);

	gdt_ptr.limit = sizeof(gdt) - 1;
	gdt_ptr.base = (uint64_t)&gdt;

	_load_gdt_tss((uintptr_t)&gdt_ptr);
}