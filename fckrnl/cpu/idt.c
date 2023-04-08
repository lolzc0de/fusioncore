#include <cpu/idt.h>
#include <asm/utils.h>

extern void _load_idt(uint64_t idt_ptr);
extern uintptr_t _isr_vector[];

static idt_desc_t idt[256];
static idt_ptr_t idt_ptr;

void idt_init(void)
{
	uint16_t i = 0;

	// Exceptions
	for (; i < 32; i++) {
		idt_create_descriptor(i, INT_GATE);
	}

	// initialize PIC before setting up PIC lines
	pic_remap();

	// standard ISA IRQs
	for (; i < 48; i++) {
		idt_create_descriptor(i, INT_GATE);
	}

	// remaining IRQs
	for (; i < 256; i++) {
		idt_create_descriptor(i, INT_GATE);
	}

	idt_ptr.limit = sizeof(idt) - 1;
	idt_ptr.base = (uint64_t)&idt;

	_load_idt((uintptr_t)&idt_ptr);

	asm volatile("sti");
}

void idt_create_descriptor(uint8_t index, uint8_t type_attribs)
{
	uint64_t offset = _isr_vector[index]; // ISR handler address

	idt[index].offset_lo = offset & 0xFFFF;
	idt[index].selector = 0x08;
	idt[index].ist = 0;
	idt[index].type_attribs = type_attribs;
	idt[index].offset_mid = (offset >> 16) & 0xFFFF;
	idt[index].offset_hi = (offset >> 32) & 0xFFFFFFFF;
	idt[index].zero = 0;
}