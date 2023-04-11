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

	for (uint8_t i = 0; i < 16; i++) {
		ioapic_set_irq_redir(lapic_get_id(), i + 32, i, true);
	}

	log(INFO, "APIC Initialized\n");
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

void lapic_signal_eoi(void)
{
	lapic_write_reg(LAPIC_EOI_REG, 0);
}

void lapic_signal_ipi(uint32_t lapic_id, uint8_t vector)
{
	lapic_write_reg(LAPIC_ICR1_REG, lapic_id << 24);
	lapic_write_reg(LAPIC_ICR0_REG, vector);
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
						    LAPIC_ENABLE_BIT |
						    SPURIOUS_INT);
}

uint8_t lapic_get_id(void)
{
	return (uint8_t)(lapic_read_reg(LAPIC_ID_REG) >> 24);
}

uint32_t ioapic_read_reg(size_t ioapic_i, uint8_t reg)
{
	size_t ioapic_i_addr = PHYS_TO_HIGHER_HALF_DATA(
		madt_ioapics[ioapic_i]->ioapic_addr);

	*((volatile uint32_t *)(ioapic_i_addr + IOREGSEL)) = reg;
	return *((volatile uint32_t *)(ioapic_i_addr + IOWIN));
}

void ioapic_write_reg(size_t ioapic_i, uint8_t reg, uint32_t data)
{
	size_t ioapic_i_addr = PHYS_TO_HIGHER_HALF_DATA(
		madt_ioapics[ioapic_i]->ioapic_addr);

	*((volatile uint32_t *)(ioapic_i_addr + IOREGSEL)) = reg;
	*((volatile uint32_t *)(ioapic_i_addr + IOWIN)) = data;
}

uint32_t ioapic_get_max_redir(size_t ioapic_i)
{
	return (ioapic_read_reg(ioapic_i, IOAPICVER_REG) & 0xFF0000) >> 16;
}

size_t ioapic_i_from_gsi(uint32_t gsi)
{
	uint32_t start_gsi;
	uint32_t end_gsi;

	for (size_t ioapic_i = 0; ioapic_i < madt_ioapics_i; ioapic_i++) {
		start_gsi = madt_ioapics[ioapic_i]->gsi_base;
		end_gsi = start_gsi + ioapic_get_max_redir(ioapic_i);

		if (gsi >= start_gsi && gsi < end_gsi) {
			return ioapic_i;
		}
	}

	log(PANIC, "Couldn't find an IOAPIC for GSI:%d!\n", gsi);

	return 0;
}

void ioapic_set_gsi_redir(uint32_t lapic_id, uint8_t vector, uint32_t gsi,
			  uint16_t flags, bool mask)
{
	uint64_t redirect_entry = vector;

	// if flags.pin_polarity is active low (else active high)
	if (flags & 2) {
		redirect_entry |= IOAPIC_PINPOL_BIT;
	}

	// if flags.trigger_mode is level triggered (else edge triggered)
	if (flags & 8) {
		redirect_entry |= IOAPIC_TRIGMODE_BIT;
	}

	if (mask) {
		redirect_entry |= IOAPIC_MASK_BIT;
	} else {
		redirect_entry &= ~IOAPIC_MASK_BIT;
	}

	// set destination
	redirect_entry |= (uint64_t)lapic_id << 56;

	size_t ioapic_i = ioapic_i_from_gsi(gsi);
	uint32_t ioredtbl =
		IRQ_TO_IOREDTBL_REG(gsi - madt_ioapics[ioapic_i]->gsi_base);

	ioapic_write_reg(ioapic_i, ioredtbl, (uint32_t)redirect_entry);
	ioapic_write_reg(ioapic_i, ioredtbl + 1,
			 (uint32_t)(redirect_entry >> 32));
}

void ioapic_set_irq_redir(uint32_t lapic_id, uint8_t vector, uint8_t irq,
			     bool mask)
{
	for (size_t isos_i = 0; isos_i < madt_isos_i; isos_i++) {
		if (madt_isos[isos_i]->irq_src == irq) {
			log(INFO, "Resolving ISO with IRQ %d\n", irq);
			ioapic_set_gsi_redir(lapic_id, vector,
						madt_isos[isos_i]->gsi,
						madt_isos[isos_i]->flags, mask);

			return;
		}
	}

	ioapic_set_gsi_redir(lapic_id, vector, irq, 0, mask);
}