#include <acpi/tbl/madt.h>

#include <cpu/cpu.h>
#include <cpu/isr.h>

#include <mm/mm.h>

#include <dd/apic/apic.h>
#include <dd/pic/pic.h>

#include <libk/serial/log.h>

static uintptr_t lapic_addr;

void apic_init(void)
{
	if (!apic_is_avail()) {
		log(PANIC, "No APIC was found on this computer!\n");
	}

	lapic_addr = PHYS_TO_HIGHER_HALF_DATA(madt->lapic_addr);

	pic_disable();
	lapic_enable();

	log(INFO, "APIC Initialized\n");
}

void lapic_signal_eoi(void)
{
	lapic_write_reg(LAPIC_EOI_REG, 0);
}

void lapic_signal_ipi(uint32_t lapic_id, uint8_t vector)
{
	lapic_write_reg(LAPIC_ICR1_REG, lapic_id << 24);
	lapic_write_reg(LAPIC_ICR0_REG, vector);
}

void ioapic_set_irq_redirect(uint32_t lapic_id, uint8_t vector, uint8_t irq)
{
}

bool apic_is_avail(void)
{
	cpuid_reg_t *regs = &(cpuid_reg_t){ .leaf = CPUID_GET_FEATURES,
					    .subleaf = 0,

					    .eax = 0,
					    .ebx = 0,
					    .ecx = 0,
					    .edx = 0 };

	cpuid(regs);

	if (regs->edx & CPUID_FEAT_EDX_APIC)
		return true;

	return false;
}

uint32_t lapic_read_reg(uint32_t reg)
{
	return *((volatile uint32_t *)(lapic_addr + reg));
}

void lapic_write_reg(uint32_t reg, uint32_t data)
{
	*((volatile uint32_t *)(lapic_addr + reg)) = data;
}

void lapic_enable(void)
{
	lapic_write_reg(LAPIC_SPURIOUS_REG, lapic_read_reg(LAPIC_SPURIOUS_REG) |
						    LAPIC_ENABLE |
						    SPURIOUS_INT);
}