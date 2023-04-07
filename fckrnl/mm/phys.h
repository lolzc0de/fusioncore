#ifndef __MM_PHYS_H_
#define __MM_PHYS_H_

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>

#include <boot/limine.h>
#include <reqs.h>

#define PAGE_SIZE 4096

void *pmm_alloc(uint64_t size);
void *pmm_allocz(uint64_t size);
void pmm_free(void *ptr, uint64_t size);
void *pmm_realloc(void *ptr, uint64_t old_size, uint64_t new_size);
void pmm_init();

uint64_t pmm_free_pages();

#endif /* __MM_PHYS_H_ */