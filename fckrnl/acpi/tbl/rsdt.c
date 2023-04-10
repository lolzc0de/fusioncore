#include <acpi/acpi.h>
#include <acpi/tbl/rsdt.h>

#include <libk/serial/debug.h>
#include <libk/serial/log.h>

#include <stdbool.h>

static rsdp_t *rsdp;
static bool has_xsdt = false;

void _acpi_rsdp_init(uint64_t rsdp_addr)
{
	// validate checksum
	_acpi_rsdp_validate(rsdp_addr);

	rsdp = (rsdp_t *)rsdp_addr;

	if (rsdp->rev >= 2) {
		has_xsdt = rsdp->xsdt_addr ? true : false;
	}
}

void _acpi_rsdp_validate(uint64_t rsdp)
{
	uint8_t cs = 0;
	uint8_t *ptr = (uint8_t *)rsdp;

	for (uint8_t i = 0; i < 20; i++) {
		cs += ptr[i];
	}

	cs = cs & 0xFF;

	if (cs == 0) {
		log(INFO, "RSDP checksum is verified\n");
	} else {
		log(PANIC, "RSDP checksum isn't 0! Checksum: 0x%x\n", cs);
	}
}

rsdp_t *_acpi_get_rsdp(void)
{
	return rsdp;
}

bool _acpi_has_xsdt(void)
{
	return has_xsdt;
}