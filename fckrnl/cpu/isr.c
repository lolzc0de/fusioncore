#include <cpu/cpu.h>
#include <cpu/isr.h>

uint64_t isr_handler(uint64_t rsp)
{
	cpu_state_t *cpu = (cpu_state_t *)rsp;

	for (;;)
		asm volatile("cli; hlt");

	return rsp;
}