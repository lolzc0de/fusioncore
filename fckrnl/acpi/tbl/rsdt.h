#ifndef __ACPI_RSDT_H_
#define __ACPI_RSDT_H_

#include <acpi/acpi.h>
#include <acpi/tbl/sdt.h>

#include <stdbool.h>

typedef struct __attribute__((__packed__)) {
	char sig[8];
	uint8_t cs;
	char oem_id[6];
	uint8_t rev;
	uint32_t rsdt_addr;

	uint32_t len;
	uint64_t xsdt_addr;
	uint8_t ext_cs;
	uint8_t reserved[3];
} rsdp_t;

typedef struct __attribute__((__packed__)) {
	sdt_t hdr;
	uint32_t entries[];
} rsdt_t;

typedef struct __attribute__((__packed__)) {
	sdt_t hdr;
	uint32_t entries[];
} xsdt_t;

void _acpi_rsdp_init(uint64_t rsdp_addr);
void _acpi_rsdp_validate(uint64_t rsdp);

rsdp_t *_acpi_get_rsdp(void);
bool _acpi_has_xsdt(void);

#endif /* __ACPI_RSDT_H_ */