#include <cpu/cpu.h>
#include <cpu/isr.h>
#include <dd/pic/pic.h>
#include <libk/serial/debug.h>
#include <libk/serial/log.h>

static const char *exceptions[] = {
	"#DE: Divide Error",
	"#DB: Debug Exception",
	" — : NMI Interrupt",
	"#BP: Breakpoint",
	"#OF: Overflow",
	"#BR: BOUND Range Exceeded",
	"#UD: Invalid Opcode (Undefined Opcode)",
	"#NM: Device Not Available (No Math Coprocessor)",
	"#DF: Double Fault",
	"— : Coprocessor Segment Overrun (reserved)",
	"#TS: Invalid TSS",
	"#NP: Segment Not Present",
	"#SS: Stack-Segment Fault",
	"#GP: General Protection",
	"#PF: Page Fault",
	"— : (Intel reserved. Do not use.)",
	"#MF: x87 FPU Floating-Point Error (Math Fault)",
	"#AC: Alignment Check",
	"#MC: Machine Check",
	"#XM: SIMD Floating-Point Exception",
	"#VE: Virtualization Exception",
	"#CP: Control Protection Exception",
	" — : Intel reserved. Do not use.",
	" — : Intel reserved. Do not use.",
	" — : Intel reserved. Do not use.",
	" — : Intel reserved. Do not use.",
	" — : Intel reserved. Do not use.",
	" — : Intel reserved. Do not use.",
	" — : Intel reserved. Do not use.",
	" — : Intel reserved. Do not use.",
	" — : Intel reserved. Do not use.",
	" — : Intel reserved. Do not use."
};

static const char *isa_irqs[] = { "Programmable Interrupt Timer",
				  "Keyboard",
				  "Cascade",
				  "COM2",
				  "COM1",
				  "LPT2",
				  "Floppy Disk",
				  "LPT1 / Unreliable spurious interrupt",
				  "CMOS real-time clock",
				  "Free for peripherals / legacy SCSI / NIC",
				  "Free for peripherals / SCSI / NIC",
				  "Free for peripherals / SCSI / NIC",
				  "PS2 Mouse",
				  "FPU / Coprocessor / Inter-processor",
				  "Primary ATA Hard Disk",
				  "Secondary ATA Hard Disk" };

uint64_t isr_handler(uint64_t rsp)
{
	cpu_state_t *cpu = (cpu_state_t *)rsp;

	if (cpu->isr_num < 32) {
		debug_set_col(TERM_RED);
		debug("\nEXCEPTION=EXCEPTION=EXCEPTION\n");
		debug("    ISR%d: %s\n", cpu->isr_num,
		      exceptions[cpu->isr_num]);
		debug("    Error Code: 0x%.16llx\n\n\n", cpu->err_code);
		debug_set_col(TERM_CYAN);
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

		debug_set_col(TERM_COLOR_RESET);
		debug("\nHalting...");

		for (;;)
			asm volatile("cli; hlt");
	} else if (cpu->isr_num >= 32 && cpu->isr_num < 48) {
		debug_set_col(TERM_RED);
		debug("\nUNKNOWN HARDWARE INTERRUPT\n\n");
		debug("    ISR%d: %s\n", cpu->isr_num,
		      exceptions[cpu->isr_num]);
		debug_set_col(TERM_CYAN);
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

		debug_set_col(TERM_COLOR_RESET);

		pic_signal_eoi(cpu->isr_num);
	} else if (cpu->isr_num == 128) {
		// TODO: Implement syscalls

		debug_set_col(TERM_RED);
		debug("\nUNKNOWN SYSCALL OCCURED\n\n");
		debug("    ISR%d: %s\n", cpu->isr_num,
		      exceptions[cpu->isr_num]);
		debug_set_col(TERM_CYAN);
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

		debug_set_col(TERM_COLOR_RESET);
	} else {
		debug_set_col(TERM_RED);
		debug("\nUNKNOWN INTERRUPT OCCURRED\n\n");
		debug("    ISR%d: %s\n", cpu->isr_num,
		      exceptions[cpu->isr_num]);
		debug_set_col(TERM_CYAN);
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

		debug_set_col(TERM_COLOR_RESET);
		debug("\nHalting...");

		for (;;)
			asm volatile("cli; hlt");
	}

	return rsp;
}