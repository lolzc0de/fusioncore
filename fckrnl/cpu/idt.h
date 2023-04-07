#ifndef __IDT_H_
#define __IDT_H_

#include <stdint.h>

#define INT_GATE 0x8E

typedef struct __attribute__((__packed__)) {
	uint16_t offset_lo;
	uint16_t selector;
	uint8_t ist;
	uint8_t type_attribs;
	uint16_t offset_mid;
	uint32_t offset_hi;
	uint32_t zero;
} idt_desc_t;

typedef struct __attribute__((__packed__)) {
	uint16_t limit;
	uint64_t base;
} idt_ptr_t;

void idt_init(void);
void idt_create_descriptor(uint8_t index, uint8_t type_attribs);

#endif /* __IDT_H_ */