#ifndef __APIC_H_
#define __APIC_H_

#include <stdbool.h>
#include <stdint.h>

#define LAPIC_EOI_REG 0x0B0
#define LAPIC_SPURIOUS_REG 0x0F0
#define LAPIC_ICR0_REG 0x300
#define LAPIC_ICR1_REG 0x310

#define LAPIC_ENABLE (1 << 8)

#define IOAPICID_REG 0
#define IOAPICVER_REG 1

#define IRQ_TO_IOREDTBL_REG(irq) (irq * 2 + 0x10)

void apic_init(void);

void lapic_signal_eoi(void);
void lapic_signal_ipi(uint32_t lapic_id, uint8_t vector);
void lapic_write_reg(uint32_t reg, uint32_t data);
uint32_t lapic_read_reg(uint32_t reg);
void lapic_enable(void);

uint32_t ioapic_read_reg(size_t ioapic_i, uint8_t reg_offset);
void ioapic_write_reg(size_t ioapic_i, uint8_t reg_offset, uint32_t data);
void ioapic_set_irq_redirect(uint32_t lapic_id, uint8_t vector, uint8_t irq,
			     bool mask);
uint32_t ioapic_max_redir(size_t ioapic_i);
size_t ioapic_i_from_gsi(uint32_t gsi);
void ioapic_set_gsi_redir(uint32_t lapic_id, uint8_t vector, uint32_t gsi,
			  uint16_t flags, bool mask);

///
bool apic_is_avail(void);

#endif /* __APIC_H_ */