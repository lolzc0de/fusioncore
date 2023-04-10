#include <cpu/cpu.h>

#include <libk/serial/debug.h>

void cpu_reg_dump(cpu_state_t *cpu)
{
	debug("Register dump:\n\n");
	debug("rax: 0x%.16llx, rbx:    0x%.16llx, rcx: 0x%.16llx, rdx: 0x%.16llx\n",
	      cpu->rax, cpu->rbx, cpu->rcx, cpu->rdx);
	debug("rsi: 0x%.16llx, rdi:    0x%.16llx, rbp: 0x%.16llx, r8:  0x%.16llx\n",
	      cpu->rsi, cpu->rdi, cpu->rbp, cpu->r8);
	debug("r9:  0x%.16llx, r10:    0x%.16llx, r11: 0x%.16llx, r12: 0x%.16llx\n",
	      cpu->r9, cpu->r10, cpu->r11, cpu->r12);
	debug("r13: 0x%.16llx, r14:    0x%.16llx, r15: 0x%.16llx, ss : 0x%.16llx\n",
	      cpu->r13, cpu->r14, cpu->r15, cpu->ss);
	debug("rsp: 0x%.16llx, rflags: 0x%.16llx, cs:  0x%.16llx, rip: 0x%.16llx\n",
	      cpu->rsp, cpu->rflags, cpu->cs, cpu->rip);
}

static inline int cpuid(cpuid_reg_t *reg)
{
	uint32_t cpuid_max;

	asm volatile("cpuid"
		     : "=a"(cpuid_max)
		     : "a"(reg->leaf & 0x80000000)
		     : "rbx", "rcx", "rdx");

	if (reg->leaf > cpuid_max)
		return 0;

	asm volatile("cpuid"
		     : "=a"(reg->eax), "=b"(reg->ebx), "=c"(reg->ecx),
		       "=d"(reg->edx)
		     : "a"(reg->leaf), "c"(reg->subleaf));

	return 1;
}

static inline char *cpu_get_vendor_str(void)
{
	cpuid_reg_t *regs = &(cpuid_reg_t){ .leaf = CPUID_GET_VENDOR_STRING,
					    .subleaf = 0,

					    .eax = 0,
					    .ebx = 0,
					    .ecx = 0,
					    .edx = 0 };

	cpuid(regs);

	char *vendor_str = malloc(13);
	snprintf(vendor_str, 13, "%.4s%.4s%.4s", (char *)&regs->ebx,
		 (char *)&regs->edx, (char *)&regs->ecx);

	return vendor_str;
}