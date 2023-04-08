#ifndef __MM_PHYS_H_
#define __MM_PHYS_H_

void pmm_init(struct stivale2_struct *stivale2_struct);
void *pmm_alloc(size_t page_cnt);
void *pmm_allocz(size_t page_cnt);
void pmm_free(void *ptr, size_t page_cnt);

const char *_get_mmap_ent_str(uint32_t type);
void *_get_ffp_range(size_t page_cnt);

#endif /* __MM_PHYS_H_ */