#ifndef __GDT_H_
#define __GDT_H_

#include <stdint.h>

#define GDT_NULL_DESCRIPTOR 0
#define GDT_KERNEL_CODE 1
#define GDT_KERNEL_DATA 2
#define GDT_USER_DATA 3
#define GDT_USER_CODE 4

#define GDT_ENTRY_COUNT 5

typedef struct __attribute__((__packed__)) {
	uint16_t length;
	uint16_t base_lo;
	uint8_t base_mid;
	uint8_t flags1;
	uint8_t flags2;
	uint8_t base_hi;
	uint32_t base_upper;
	uint32_t reserved;
} tss_entry_t;

typedef struct __attribute__((__packed__)) {
	uint16_t limit_lo;
	uint16_t base_lo;
	uint8_t base_mid;
	uint8_t access;
	uint8_t flags;
	uint8_t base_hi;
} gdt_entry_t;

typedef struct __attribute__((__packed__)) {
	gdt_entry_t entries[GDT_ENTRY_COUNT];
	tss_entry_t gdt_tss;
} gdt_t;

typedef struct __attribute__((__packed__)) {
	uint16_t limit;
	uint64_t base;
} gdt_desc_t;

void gdt_init();
gdt_t *gdt_get(void);
gdt_desc_t *gdt_get_descriptor(void);

#endif /* __GDT_H_ */