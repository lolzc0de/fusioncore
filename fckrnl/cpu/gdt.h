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
	uint32_t reserved0;
    uint64_t rsp[3];
    uint64_t reserved1;
    uint64_t ist[7];
    uint32_t reserved2;
    uint32_t reserved3;
    uint16_t reserved4;
	uint16_t iopb_offset;
} tss_t;

typedef struct __attribute__((__packed__)) {
	uint16_t length;
	uint16_t base_lo;
	uint8_t base_mid;
	uint8_t flags1;
	uint8_t flags2;
	uint8_t base_hi;
	uint32_t base_upper;
	uint32_t reserved;
} tss_desc_t;

typedef struct __attribute__((__packed__)) {
	uint16_t limit_lo;
	uint16_t base_lo;
	uint8_t base_mid;
	uint8_t access;
	uint8_t flags;
	uint8_t base_hi;
} gdt_desc_t;

typedef struct __attribute__((__packed__)) {
	gdt_desc_t entries[GDT_ENTRY_COUNT];
	tss_desc_t tss_descriptor;
} gdt_t;

typedef struct __attribute__((__packed__)) {
	uint16_t limit;
	uint64_t base;
} gdt_ptr_t;

void gdt_init();

#endif /* __GDT_H_ */