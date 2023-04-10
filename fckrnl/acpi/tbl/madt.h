#ifndef __ACPI_MADT_H_
#define __ACPI_MADT_H_

#include <acpi/tbl/sdt.h>

#include <stddef.h>

typedef struct __attribute__((__packed__)) {
	uint8_t type;
	uint8_t length;
} madt_header_t;

typedef struct __attribute__((__packed__)) {
	madt_header_t hdr;
	uint8_t acpi_proc_id;
	uint8_t acpi_id;
	uint32_t flags;
} madt_lapic_t;

typedef struct __attribute__((__packed__)) {
	madt_header_t hdr;
	uint8_t ioapic_id;
	uint8_t reserved;
	uint32_t ioapic_addr;
	uint32_t gsi_base;
} madt_ioapic_t;

typedef struct __attribute__((__packed__)) {
	madt_header_t hdr;
	uint8_t bus_src;
	uint8_t irq_src;
	uint32_t gsi;
	uint16_t flags;
} madt_iso_t;

typedef struct __attribute__((__packed__)) {
	madt_header_t hdr;
	uint8_t acpi_proc_id;
	uint16_t flags;
	uint8_t lint;
} madt_lapic_nmi_t;

typedef struct __attribute__((__packed__)) {
	sdt_t hdr;
	uint32_t lapic_addr;
	uint32_t flags;
	madt_header_t entries[];
} madt_t;

void _acpi_madt_init(void);

extern madt_t *madt;

extern madt_lapic_t **madt_lapics;
extern madt_ioapic_t **madt_ioapics;
extern madt_iso_t **madt_isos;
extern madt_lapic_nmi_t **madt_lapic_nmis;

extern size_t madt_lapics_i;
extern size_t madt_ioapics_i;
extern size_t madt_isos_i;
extern size_t madt_lapic_nmis_i;

void _acpi_madt_init(void);

#endif /* __ACPI_MADT_H_ */