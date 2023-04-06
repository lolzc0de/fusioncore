#include <boot/bootinfo.h>
#include <boot/efi_mem.h>
#include <mm/mm.h>
#include <mm/bitmap.h>
#include <mm/pf_alloc.h>
#include <dd/fb/fb.h>

extern uint64_t _kern_start;
extern uint64_t _kern_end;


// !!!!!!!!!!!!! THIS IS A TEMPORARY SOLUTION
typedef struct {
	unsigned int x;
	unsigned int y;
} point;

char intTo_StringOutput[128];
const char *to_string(uint64_t value)
{
	uint8_t size;
	uint64_t size_test = value;
	while (size_test / 10 > 0) {
		size_test /= 10;
		size++;
	}

	uint8_t index = 0;
	while (value / 10 > 0) {
		uint8_t remainder = value % 10;
		value /= 10;
		intTo_StringOutput[size - index] = remainder + '0';
		index++;
	}

	uint8_t remainder = value % 10;
	intTo_StringOutput[size - index] = remainder = '0';
	intTo_StringOutput[size - index + 1] = 0;
	return intTo_StringOutput;
}

void putchar(framebuffer_t *framebuffer, PSF1_FONT *font, char chr, unsigned int x_off, unsigned int y_off)
{
	unsigned int *p_pixel = (unsigned int *)framebuffer->base_addr;
	char *p_font = font->glyph_buf + (chr * font->header->charsize);
	
	for (unsigned long y = y_off; y < y_off + 16; y++) {
		for (unsigned long x = x_off; x < x_off + 8; x++) {
			if ((*p_font & (0b10000000 >> (x - x_off))) > 0) {
				*(unsigned int *)(p_pixel + x + (y * framebuffer->pixels_per_scanline)) = 0xFFFFFFFF;
			}
		}
		p_font++;
	}
}

point cur_pos;

void print(framebuffer_t *framebuffer, PSF1_FONT *font, char *str)
{
	char *chr = str;
	while (*chr != 0) {
		putchar(framebuffer, font, *chr, cur_pos.x, cur_pos.y);
		cur_pos.x += 8;
		if (cur_pos.x + 8 > framebuffer->width) {
			cur_pos.x = 0;
			cur_pos.y += 16;
		}
		chr++;
	}
}
// !!!!!!!!!!!!! THIS IS A TEMPORARY SOLUTION

void _start(boot_info_t *boot_info)
{
	mm_read_efi_mmap(boot_info->mmap, boot_info->mmap_size, boot_info->mmap_desc_size);

	cur_pos.x = 0; cur_pos.y = 0;
	print(boot_info->framebuffer, boot_info->font, "Free RAM: ");
	print(boot_info->framebuffer, boot_info->font, to_string(mm_get_free_mem() / 1024));
	print(boot_info->framebuffer, boot_info->font, "KB");
	cur_pos.x = 0; cur_pos.y += 16;

	cur_pos.x = 0; cur_pos.y = 0;
	print(boot_info->framebuffer, boot_info->font, "Used RAM: ");
	print(boot_info->framebuffer, boot_info->font, to_string(mm_get_used_mem() / 1024));
	print(boot_info->framebuffer, boot_info->font, "KB");
	cur_pos.x = 0; cur_pos.y += 16;

	cur_pos.x = 0; cur_pos.y = 0;
	print(boot_info->framebuffer, boot_info->font, "Reserved RAM: ");
	print(boot_info->framebuffer, boot_info->font, to_string(mm_get_reserved_mem() / 1024));
	print(boot_info->framebuffer, boot_info->font, "KB");
	cur_pos.x = 0; cur_pos.y += 16;

	//printf("Free Memory: %i KB\n", mm_get_free_mem(&allocator) / 1024);
	//printf("Used Memory: %i KB\n", mm_get_used_mem(&allocator) / 1024);
	//printf("Reserved Memory: %i KB\n", mm_get_reserved_mem(&allocator) / 1024);

	// reserve pages used by kernel
	uint64_t kernel_size = (uint64_t)&_kern_end - (uint64_t)&_kern_start;
	uint64_t kernel_pages = (uint64_t)&kernel_size / 4096 + 1;

	mm_lock_pages(&_kern_start, kernel_pages);
}