#include <asm/utils.h>
#include <boot/stivale2.h>
#include <boot/boot.h>
#include <libk/serial/log.h>
#include <libk/assert.h>
#include <libk/string.h>
#include <mm/mm.h>
#include <mm/phys.h>
#include <mm/virt.h>

static uint64_t *root_page_tbl;

void vmm_init(struct stivale2_struct *stivale2_struct)
{
	struct stivale2_struct_tag_memmap *mmap = stivale2_get_tag(
		stivale2_struct, STIVALE2_STRUCT_TAG_MEMMAP_ID);
	struct stivale2_mmap_entry *cur_entry;

	root_page_tbl = PHYS_TO_HIGHER_HALF_DATA(pmm_allocz(1));
	assert(root_page_tbl != NULL);

	vmm_map_range(root_page_tbl, 0, 4 * GiB, 0, KERNEL_READ_WRITE);
	vmm_map_range(root_page_tbl, 0, 4 * GiB, HIGHER_HALF_DATA,
		      KERNEL_READ_WRITE);
	vmm_map_range(root_page_tbl, 0, 2 * GiB, HIGHER_HALF_CODE, KERNEL_READ);

	for (uint64_t i = 0; i < mmap->entries; i++) {
		cur_entry = &mmap->memmap[i];
		vmm_map_range(root_page_tbl, 0, cur_entry->length,
			      HIGHER_HALF_DATA, KERNEL_READ_WRITE);
	}

	log(INFO, "Replaced bootloader page table at 0x%.16llx\n", read_cr(3));
	vmm_load_page_tbl(root_page_tbl);
	log(INFO, "Now using kernel page table at 0x%.16llx\n", read_cr(3));

	log(INFO, "VMM initialized\n");
}

void vmm_map_page(uint64_t *page_tbl, uint64_t phys_page, uint64_t virt_page,
		  uint64_t flags)
{
	uint64_t pt_value = phys_page | flags;
	vmm_set_pt_value(page_tbl, ALIGN_DOWN(virt_page, 4096), flags,
			 pt_value);
}

void vmm_unmap_page(uint64_t *page_tbl, uint64_t virt_page)
{
	uint64_t pt_val = 0;
	vmm_set_pt_value(page_tbl, ALIGN_DOWN(virt_page, 4096), 0, pt_val);
}

void vmm_map_range(uint64_t *page_tbl, uint64_t start, uint64_t end,
		   uint64_t offset, uint64_t flags)
{
	for (uint64_t i = ALIGN_DOWN(start, 4096); i < ALIGN_UP(end, 4096);
	     i += PAGE_SIZE) {
		vmm_map_page(page_tbl, i, i + offset, flags);
	}
}

void vmm_unmap_range(uint64_t *page_tbl, uint64_t start, uint64_t end)
{
	for (uint64_t i = ALIGN_DOWN(start, 4096); i < ALIGN_UP(end, 4096);
	     i += PAGE_SIZE) {
		vmm_unmap_page(page_tbl, i);
	}
}

uint64_t *vmm_get_or_create_pml(uint64_t *pml, size_t pml_index, uint64_t flags)
{
	// check present flag
	if (!(pml[pml_index] & 1)) {
		pml[pml_index] =
			(uint64_t)pmm_allocz(1) |
			flags;
	}

	return (uint64_t *)(pml[pml_index] & ~(511));
}

void vmm_set_pt_value(uint64_t *page_tbl, uint64_t virt_page, uint64_t flags,
		      uint64_t value)
{
	// index for page mapping level 4
	size_t pml4_index = (virt_page & ((uintptr_t)0x1ff << 39)) >> 39;
	// index for page directory table
	size_t pdpt_index = (virt_page & ((uintptr_t)0x1ff << 30)) >> 30;
	// index for page directory
	size_t pd_index = (virt_page & ((uintptr_t)0x1ff << 21)) >> 21;
	// index for page table
	size_t pt_index = (virt_page & ((uintptr_t)0x1ff << 12)) >> 12;

	// page mapping level 4 = pml4
	uint64_t *pml4 = page_tbl;
	// page directory table = pml3
	uint64_t *pdpt = vmm_get_or_create_pml(pml4, pml4_index, flags);
	// page directory = pml2
	uint64_t *pd = vmm_get_or_create_pml(pdpt, pdpt_index, flags);
	// page table = pml1
	uint64_t *pt = vmm_get_or_create_pml(pd, pd_index, flags);

	// actual mapped value (either physical frame address or 0)
	pt[pt_index] = value;

	// for changes to apply, the translation lookaside buffers need to be flushed
	vmm_flush_tlb((void *)virt_page);
}

uint64_t *vmm_get_root_page_tbl(void)
{
	return root_page_tbl;
}

void vmm_flush_tlb(void *address)
{
	invlpg(address);
}

void vmm_load_page_tbl(uint64_t *page_tbl)
{
	write_cr(3, HIGHER_HALF_DATA_TO_PHYS((uint64_t)page_tbl));
}