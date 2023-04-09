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

		cpu_reg_dump(cpu);

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

		cpu_reg_dump(cpu);

		debug_set_col(TERM_COLOR_RESET);

		pic_signal_eoi(cpu->isr_num);
	} else if (cpu->isr_num == SYSCALL_INT) {
		// TODO: Implement syscalls

		debug_set_col(TERM_RED);
		debug("\nUNKNOWN SYSCALL OCCURED\n\n");
		debug_set_col(TERM_CYAN);

		cpu_reg_dump(cpu);

		debug_set_col(TERM_COLOR_RESET);
	} else if (cpu->isr_num == SPURIOUS_INT) {
		// TODO: Spurious interrupts

		debug("\nSPURIOUS INTERRUPT OCCURED\n\n");
		debug_set_col(TERM_CYAN);

		cpu_reg_dump(cpu);

		debug_set_col(TERM_COLOR_RESET);
	} else {
		debug_set_col(TERM_RED);
		debug("\nUNKNOWN INTERRUPT OCCURRED\n\n");
		debug("    ISR%d: %s\n", cpu->isr_num,
		      exceptions[cpu->isr_num]);
		debug_set_col(TERM_CYAN);

		cpu_reg_dump(cpu);

		debug_set_col(TERM_COLOR_RESET);
		debug("\nHalting...");

		for (;;)
			asm volatile("cli; hlt");
	}

	return rsp;
}