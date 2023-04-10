#ifndef __APIC_H_
#define __APIC_H_

#include <stdbool.h>
#include <stdint.h>

#define LAPIC_EOI_REG 0x0B0
#define LAPIC_SPURIOUS_REG 0x0F0
#define LAPIC_ICR0_REG 0x300
#define LAPIC_ICR1_REG 0x310

#define LAPIC_ENABLE (1 << 8)

void apic_init(void);
void lapic_signal_eoi(void);
void lapic_signal_ipi(uint32_t lapic_id, uint8_t vector);
void ioapic_set_irq_redirect(uint32_t lapic_id, uint8_t vector, uint8_t irq);

///
bool apic_is_avail(void);
uint32_t lapic_read_reg(uint32_t reg);
void lapic_write_reg(uint32_t reg, uint32_t data);
void lapic_enable(void);
void ioapic_set_gsi_redirect(void);

#endif /* __APIC_H_ */