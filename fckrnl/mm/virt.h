#ifndef __MM_VIRT_H_
#define __MM_VIRT_H_

#define PTE_PRESENT 1
#define PTE_READ_WRITE (1 << 1)
#define PTE_USER_SUPERVISOR (1 << 2)
#define PTE_WRITE_THROUGH (1 << 3)
#define PTE_CHACHE_DISABLED (1 << 4)
#define PTE_ACCESSED (1 << 5)
#define PTE_DIRTY (1 << 6)
#define PTE_PAT (1 << 7)
#define PTE_GLOBAL (1 << 8)

typedef enum {
	KERNEL_READ = PTE_PRESENT,
	KERNEL_READ_WRITE = PTE_PRESENT | PTE_READ_WRITE,
	USER_READ = PTE_PRESENT | PTE_USER_SUPERVISOR,
	USER_READ_WRITE = PTE_PRESENT | PTE_READ_WRITE | PTE_USER_SUPERVISOR
} vmm_map_priv_t;

void vmm_init(struct stivale2_struct *stivale2_struct);
void vmm_map_page(uint64_t *page_tbl, uint64_t phys_page, uint64_t virt_page,
		  uint64_t flags);
void vmm_unmap_page(uint64_t *page_tbl, uint64_t virt_page);
void vmm_map_range(uint64_t *page_tbl, uint64_t start, uint64_t end,
		   uint64_t offset, uint64_t flags);
void vmm_unmap_range(uint64_t *page_tbl, uint64_t start, uint64_t end);

///
uint64_t *vmm_get_or_create_pml(uint64_t *pml, size_t pml_index, uint64_t flags);
void vmm_set_pt_value(uint64_t *page_tbl, uint64_t virt_page, uint64_t flags, uint64_t value);
void vmm_flush_tlb(void *addr);
void vmm_load_page_tbl(uint64_t *page_tbl);

#endif /* __MM_VIRT_H_ */