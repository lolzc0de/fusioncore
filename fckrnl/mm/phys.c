#include <boot/stivale2.h>
#include <boot/boot.h>
#include <libk/serial/debug.h>
#include <libk/serial/log.h>
#include <libk/string.h>
#include <mm/mm.h>
#include <mm/bmap.h>
#include <mm/phys.h>

bitmap_t pmm_bitmap;
static size_t hi_page_top = 0;
static size_t used_page_cnt = 0;

void pmm_init(struct stivale2_struct *stivale2_struct)
{
	struct stivale2_struct_tag_memmap *mmap = stivale2_get_tag(
		stivale2_struct, STIVALE2_STRUCT_TAG_MEMMAP_ID);
	struct stivale2_mmap_entry *cur_ent;

	size_t cur_page_top = 0;

	log(INFO, "Memory map layout:\n");

	for (uint64_t i = 0; i < mmap->entries; i++) {
		cur_ent = &mmap->memmap[i];

		debug_set_col(TERM_PURPLE);
		debug("Memory map entry %.16d:\n", i);
		debug("Base: 0x%.16llx, Length: 0x%.16llx, Type: %s\n",
		      cur_ent->base, cur_ent->length,
		      _get_mmap_ent_str(cur_ent->type));
		debug_set_col(TERM_COLOR_RESET);

		if (cur_ent->type != STIVALE2_MMAP_USABLE &&
		    cur_ent->type != STIVALE2_MMAP_BOOTLOADER_RECLAIMABLE &&
		    cur_ent->type != STIVALE2_MMAP_KERNEL_AND_MODULES)
			continue;

		cur_page_top = cur_ent->base + cur_ent->length;

		if (cur_page_top > hi_page_top)
			hi_page_top = cur_page_top;
	}

	used_page_cnt = KB_TO_PAGES(hi_page_top);

	pmm_bitmap.size = ALIGN_UP(
		ALIGN_DOWN(hi_page_top, PAGE_SIZE) / PAGE_SIZE / 8, PAGE_SIZE);

	for (uint64_t i = 0; i < mmap->entries; i++) {
		cur_ent = &mmap->memmap[i];

		if (cur_ent->type != STIVALE2_MMAP_USABLE)
			continue;

		if (cur_ent->length >= pmm_bitmap.size) {
			pmm_bitmap.map =
				(uint8_t *)(phys_to_hhd(cur_ent->base));

			cur_ent->base += pmm_bitmap.size;
			cur_ent->length -= pmm_bitmap.size;

			break;
		}
	}

	memset((void *)pmm_bitmap.map, 0xFF, pmm_bitmap.size);

	for (uint64_t i = 0; i < mmap->entries; i++) {
		cur_ent = &mmap->memmap[i];

		if (cur_ent->type == STIVALE2_MMAP_USABLE)
			pmm_free((void *)cur_ent->base,
				 cur_ent->length / PAGE_SIZE);
	}

	bitmap_set(&pmm_bitmap, 0);

	log(INFO, "PMM initialized\n");
}

void *pmm_alloc(size_t page_count)
{
	if (used_page_cnt <= 0)
		return NULL;

	void *ptr = _get_ffp_range(page_count);

	if (ptr == NULL)
		return NULL;

	uint64_t index = PAGE_TO_BIT(ptr);

	for (size_t i = 0; i < page_count; i++) {
		bitmap_set(&pmm_bitmap, index + i);
	}

	used_page_cnt += page_count;

	return (void *)BIT_TO_PAGE(index);
}

void *pmm_allocz(size_t page_count)
{
	if (used_page_cnt <= 0)
		return NULL;

	void *ptr = pmm_ffp_range(page_count);
	if (ptr == NULL)
		return NULL;

	memset(ptr, 0, PAGE_SIZE * page_count);

	uint64_t index = PAGE_TO_BIT(ptr);

	for (size_t i = 0; i < page_count; i++)
		bitmap_set(&pmm_bitmap, index + i);

	used_page_cnt += page_count;

	return (void *)BIT_TO_PAGE(index);
}

void pmm_free(void *ptr, size_t page_count)
{
	uint64_t index = PAGE_TO_BIT(ptr);

	for (size_t i = 0; i < page_count; i++)
		bitmap_clear(&pmm_bitmap, index + i);

	used_page_cnt -= page_count;
}

const char *_get_mmap_ent_str(uint32_t type)
{
	switch (type) {
	case STIVALE2_MMAP_USABLE:
		return "Usable";
	case STIVALE2_MMAP_RESERVED:
		return "Reserved";
	case STIVALE2_MMAP_ACPI_RECLAIMABLE:
		return "ACPI Reclaimable";
	case STIVALE2_MMAP_ACPI_NVS:
		return "ACPI Non Volatile Storage";
	case STIVALE2_MMAP_BAD_MEMORY:
		return "Bad Memory";
	case STIVALE2_MMAP_BOOTLOADER_RECLAIMABLE:
		return "Bootloader Reclaimable";
	case STIVALE2_MMAP_KERNEL_AND_MODULES:
		return "Kernel And Modules";
	case STIVALE2_MMAP_FRAMEBUFFER:
		return "Framebuffer";
	default:
		return "Unknown";
	}
}

void *_get_ffp_range(size_t page_count)
{
	for (size_t all_bits_i = 0; all_bits_i < PAGE_TO_BIT(hi_page_top);
	     all_bits_i++) {
		for (size_t page_count_i = 0; page_count_i < page_count;
		     page_count_i++) {
			if (bitmap_get(&pmm_bitmap,
				       all_bits_i + PAGE_TO_BIT(page_count_i)))
				break;

			if (page_count_i == page_count - 1)
				return (void *)BIT_TO_PAGE(all_bits_i);
		}
	}

	return NULL;
}