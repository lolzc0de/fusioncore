#include <cpu/gdt.h>

static gdt_t gdt;
static gdt_desc_t gdt_descriptor;

extern void _load_gdt_tss(uint64_t gdt_desc_ptr);

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
	gdt.gdt_tss.length = 104;
	gdt.gdt_tss.base_lo = 0;
	gdt.gdt_tss.base_mid = 0;
	gdt.gdt_tss.flags1 = 0b10001001;
	gdt.gdt_tss.flags2 = 0;
	gdt.gdt_tss.base_hi = 0;
	gdt.gdt_tss.base_upper = 0;
	gdt.gdt_tss.reserved = 0;

	gdt_descriptor.limit = sizeof(gdt) - 1;
	gdt_descriptor.base = (uint64_t)&gdt;

	_load_gdt_tss((uintptr_t)&gdt_descriptor);
}

gdt_t *gdt_get(void)
{
	return &gdt;
}

gdt_desc_t *gdt_get_descriptor(void)
{
	return &gdt_descriptor;
}
