#include <asm/utils.h>
#include <boot/stivale2.h>
#include <boot/boot.h>
#include <libk/serial/log.h>
#include <libk/assert.h>
#include <mm/mm.h>
#include <mm/phys.h>
#include <mm/virt.h>

static uint64_t *root_page_tbl;

void vmm_init(void)
{
	root_page_tbl = pmm_alloc(1);
	assert(root_page_tbl != NULL);
	memset(root_page_tbl, 0, PAGE_SIZE);

	for (uint64_t i = 0; i < 4 * GB; i += PAGE_SIZE) {
		vmm_map_page(root_page_tbl, i, i, PTE_PRESENT | PTE_READ_WRITE);
	}

	//vmm_map_range(root_page_tbl, 0, 4 * GB, 0, PTE_PRESENT | PTE_READ_WRITE);
	//vmm_map_range(root_page_tbl, 0, 4 * GB, HIGHER_HALF_DATA_LV4, PTE_PRESENT | PTE_READ_WRITE);
	//vmm_map_range(root_page_tbl, 0, 2 * GB, HIGHER_HALF_DATA_LV4, PTE_PRESENT);

	//vmm_load_page_tbl(root_page_tbl);

	log(INFO, "VMM initialized\n");
}

void vmm_map_page(uint64_t *page_tbl, uint64_t phys_page, uint64_t virt_page,
		  uint64_t flags)
{
	//// index for page mapping level 4
	//size_t pml4_index	= (virt_page & ((uintptr_t)0x1ff << 39)) >> 39;
	//// index for page directory table
	//size_t pdpt_index	= (virt_page & ((uintptr_t)0x1ff << 30)) >> 30;
	//// index for page directory
	//size_t pd_index	= (virt_page & ((uintptr_t)0x1ff << 21)) >> 21;
	//// index for page table
	//size_t pt_index	= (virt_page & ((uintptr_t)0x1ff << 12)) >> 12;

	//// page mapping level 4 = pml4
	//uint64_t *pml4  = page_tbl;
	//// page directory table = pml3
	//uint64_t *pdpt  = vmm_get_or_create_pml(pml4, pml4_index, flags);
	//// page directory	    = pml2
	//uint64_t *pd    = vmm_get_or_create_pml(pdpt, pdpt_index, flags);	// <---
	//// page table	    = pml1
	//uint64_t *pt    = vmm_get_or_create_pml(pd, pd_index, flags);

	//// actual mapped, physical frame
	//pt[pt_index]    = phys_page | flags;

	//// for changes to apply, the translation lookaside buffers need to be flushed
	//vmm_flush_tlb((void *)virt_page);

	uint64_t pt_value = phys_page | flags;
	vmm_set_pt_value(page_tbl, virt_page, flags, pt_value);
}

//void vmm_unmap_page(uint64_t *page_tbl, uint64_t virt_page)
//{
//    // // index for page mapping level 4
//    // size_t pml4_index	= (virt_page & ((uintptr_t)0x1ff << 39)) >> 39;
//    // // index for page directory table
//    // size_t pdpt_index	= (virt_page & ((uintptr_t)0x1ff << 30)) >> 30;
//    // // index for page directory
//    // size_t pd_index	= (virt_page & ((uintptr_t)0x1ff << 21)) >> 21;
//    // // index for page table
//    // size_t pt_index	= (virt_page & ((uintptr_t)0x1ff << 12)) >> 12;
//
//    // // page mapping level 4 = pml4
//    // uint64_t *pml4  = page_tbl;
//    // // page directory table = pml3
//    // uint64_t *pdpt  = vmm_get_or_create_pml(pml4, pml4_index, 0);
//    // // page directory	    = pml2
//    // uint64_t *pd    = vmm_get_or_create_pml(pdpt, pdpt_index, 0);
//    // // page table	    = pml1
//    // uint64_t *pt    = vmm_get_or_create_pml(pd, pd_index, 0);
//
//    // // unset mapped, physical frame
//    // pt[pt_index]    = 0;
//
//    // // for changes to apply, the translation lookaside buffers need to be flushed
//    // vmm_flush_tlb((void *)virt_page);
//
//    uint64_t pt_value = 0;
//    vmm_set_pt_value(page_tbl, virt_page, 0, pt_value);
//}

void vmm_map_range(uint64_t *page_tbl, uint64_t start, uint64_t end,
		   uint64_t offset, uint64_t flags)
{
	assert(IS_PAGE_ALIGNED(start));
	assert(IS_PAGE_ALIGNED(end));

	for (uint64_t i = start; i < end; i += PAGE_SIZE) {
		vmm_map_page(page_tbl, i, i + offset, flags);
	}
}

uint64_t *vmm_get_or_create_pml(uint64_t *pml, size_t pml_index, uint64_t flags)
{
	// check present flag
	if (!(pml[pml_index] & 1)) {
		pml[pml_index] = phys_to_hhd((uint64_t)pmm_alloc(1)) | flags;
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

void vmm_flush_tlb(void *address)
{
	invlpg(address);
}

void vmm_load_page_tbl(uint64_t *page_tbl)
{
	write_cr(3, hhd_to_phys((uint64_t)page_tbl));
}