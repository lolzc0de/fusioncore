#ifndef __MM_MM_H_
#define __MM_MM_H_

#include <stdbool.h>

#define HIGHER_HALF_DATA 0xFFFF800000000000UL
#define HIGHER_HALF_CODE 0xFFFFFFFF80000000UL
#define HIGHER_HALF_END 0xFFFFFFFFFFFFFFFFUL

#define GiB 0x40000000UL

#define HEAP_MAX_SIZE (4 * GiB)
#define HEAP_START_ADDR 0xFFFF900000000000

#define PAGE_SIZE 4096

#define KB_TO_PAGES(kb) (((kb)*1024) / PAGE_SIZE)
#define ALIGN_UP(addr, align) (((addr) + (align)-1) & ~((align)-1))
#define ALIGN_DOWN(addr, align) ((addr) & ~((align)-1))

#define BIT_TO_PAGE(bit) ((size_t)bit * PAGE_SIZE)
#define PAGE_TO_BIT(page) ((size_t)page / PAGE_SIZE)

#define PHYS_TO_HIGHER_HALF_DATA(addr) (addr + HIGHER_HALF_DATA)
#define PHYS_TO_HIGHER_HALF_CODE(addr) (addr + HIGHER_HALF_CODE)
#define HIGHER_HALF_DATA_TO_PHYS(addr) (addr - HIGHER_HALF_DATA)
#define HIGHER_HALF_CODE_TO_PHYS(addr) (addr - HIGHER_HALF_CODE)

#endif /* __MM_MM_H_ */