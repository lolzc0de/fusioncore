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
						    LAPIC_ENABLE |
						    SPURIOUS_INT);
}

uint32_t ioapic_read_reg(size_t ioapic_i, uint8_t reg_offset)
{
	uint32_t volatile *cur_ioapic_base =
		(uint32_t volatile *)madt_ioapics[ioapic_i];
	*cur_ioapic_base = reg_offset;
	return *(cur_ioapic_base + 0x10);
}

void ioapic_write_reg(size_t ioapic_i, uint8_t reg_offset, uint32_t data)
{
	uint32_t volatile *cur_ioapic_base =
		(uint32_t volatile *)madt_ioapics[ioapic_i];
	*cur_ioapic_base = reg_offset;
	*(cur_ioapic_base + 0x10) = data;
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
		redirect_entry |= 1 << 13;
	}

	// if flags.trigger_mode is level triggered (else edge triggered)
	if (flags & 8) {
		redirect_entry |= 1 << 15;
	}

	if (!mask) {
		redirect_entry |= 1 << 16;
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