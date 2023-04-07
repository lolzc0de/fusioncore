#include <boot/limine.h>
#include <cpu/spinlock.h>
#include <mm/phys.h>
#include <mm/virt.h>
#include <dd/fb/fb.h>

extern uint64_t _kern_start;
extern uint64_t _kern_end;

volatile struct limine_kernel_address_request kernel_address_request = {
    .id = LIMINE_KERNEL_ADDRESS_REQUEST,
    .revision = 0
};
volatile struct limine_hhdm_request hhdm_request = {
    .id = LIMINE_HHDM_REQUEST,
    .revision = 0
};
volatile struct limine_memmap_request memmap_request = {
    .id = LIMINE_MEMMAP_REQUEST,
    .revision = 0
};
volatile struct limine_stack_size_request stack_size_request = {
    .id = LIMINE_STACK_SIZE_REQUEST,
    .revision = 0
};
/*static volatile struct limine_framebuffer_request framebuffer_request = {
    .id = LIMINE_FRAMEBUFFER_REQUEST,
    .revision = 0
};*/

void _start(void)
{
	// TODO: Load GDT and IDT
	//tbl_init();
	pmm_init();

	// Here we keep triplefaulting for some reason
	// I suspect it's because we don't have GDT loaded yet
	//vmm_init();

	asm("cli");
		for (;;) asm("hlt");
}