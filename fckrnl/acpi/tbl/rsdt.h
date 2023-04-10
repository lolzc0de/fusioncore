#ifndef __ACPI_RSDT_H_
#define __ACPI_RSDT_H_

#include <acpi/acpi.h>
#include <acpi/tbl/sdt.h>

typedef struct __attribute__((__packed__)) {
	char signature[8];
	uint8_t checksum;
	char oem_id[6];
	uint8_t rev;
	uint32_t rsdt_addr;

	uint32_t length;
	uint64_t xsdt_addr;
	uint8_t ext_checksum;
	uint8_t reserved[3];
} rsdp_t;

typedef struct __attribute__((__packed__)) {
	sdt_t hdr;
	uint32_t entries;
} rsdt_t;

typedef struct __attribute__((__packed__)) {
	sdt_t hdr;
	uint32_t entries;
} xsdt_t;

void _acpi_rsdp_init(uint64_t rsdp_addr);
void _acpi_rsdp_validate(uint64_t rsdp);

#endif /* __ACPI_RSDT_H_ */