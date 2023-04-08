#ifndef __MM_MM_H_
#define __MM_MM_H_

#include <stdbool.h>

#define HIGHER_HALF_DATA_LV5 0xFF00000000000000UL
#define HIGHER_HALF_DATA_LV4 0xFFFF800000000000UL
#define HIGHER_HALF_CODE 0xFFFFFFFF80000000UL

#define PAGE_SIZE 4096
#define TABLES_PER_DIR 512
#define PAGES_PER_TBL 512 // NOTE: Is this needed?

#define KB_TO_PAGES(kb) (((kb)*1024) / PAGE_SIZE)
#define ALIGN_DOWN(addr, align) ((addr) & ~((align)-1))
#define ALIGN_UP(addr, align) (((addr) + (align)-1) & ~((align)-1))

#define IS_PAGE_ALIGNED(num) ((num % PAGE_SIZE) == 0)

#define BIT_TO_PAGE(bit) ((size_t)bit * PAGE_SIZE)
#define PAGE_TO_BIT(page) ((size_t)page / PAGE_SIZE)

static inline bool is_la57_enabled(void)
{
	uint64_t cr4;
	asm volatile("mov %%cr4, %0" : "=rax"(cr4));
	return (cr4 >> 12) & 1;
}

// phys_to_higher_half_data
static inline uintptr_t phys_to_hhd(uintptr_t address)
{
	if (is_la57_enabled())
		return HIGHER_HALF_DATA_LV5 + address;
	return HIGHER_HALF_DATA_LV4 + address;
}

// phys_to_higher_half_code
static inline uintptr_t phys_to_hhc(uintptr_t address)
{
	return HIGHER_HALF_CODE + address;
}

// higher_half_data_to_phys
static inline uintptr_t hhd_to_phys(uintptr_t address)
{
	if (is_la57_enabled())
		return address - HIGHER_HALF_DATA_LV5;
	return address - HIGHER_HALF_DATA_LV4;
}

// higher_half_code_to_phys
static inline uintptr_t hhc_to_phys(uintptr_t address)
{
	return address - HIGHER_HALF_CODE;
}

#endif /* __MM_MM_H_ */