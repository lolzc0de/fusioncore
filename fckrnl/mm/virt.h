#ifndef __MM_VIRT_H_
#define __MM_VIRT_H_

#define GB 0x40000000UL

#define PTE_PRESENT 1
#define PTE_READ_WRITE 2
#define PTE_USER_SUPERVISOR 4
#define PTE_WRITE_THROUGH 8
#define PTE_CHACHE_DISABLED 16
#define PTE_ACCESSED 32
#define PTE_DIRTY 64
#define PTE_PAT 128
#define PTE_GLOBAL 256

void vmm_init(void);
void vmm_map_page(uint64_t *page_tbl, uint64_t phys_page, uint64_t virt_page,
		  uint64_t flags);
void vmm_unmap_page(uint64_t *page_tbl, uint64_t virt_page);
void vmm_map_range(uint64_t *page_tbl, uint64_t start, uint64_t end,
		   uint64_t offset, uint64_t flags);

#endif /* __MM_VIRT_H_ */