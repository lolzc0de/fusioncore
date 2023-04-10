#include <boot/stivale2.h>
#include <boot/boot.h>

#include <acpi/acpi.h>
#include <acpi/tbl/sdt.h>
#include <acpi/tbl/rsdt.h>
#include <acpi/tbl/madt.h>

#include <mm/mm.h>

#include <libk/serial/log.h>
#include <libk/string.h>

#include <stdbool.h>

static rsdt_t *rsdt;
static xsdt_t *xsdt;

void acpi_init(struct stivale2_struct *stivale2_struct)
{
	struct stivale2_struct_tag_rsdp *rsdp_tag =
		stivale2_get_tag(stivale2_struct, STIVALE2_STRUCT_TAG_RSDP_ID);

	_acpi_rsdp_init(rsdp_tag->rsdp);

	if (_acpi_has_xsdt()) {
		xsdt = (xsdt_t *)PHYS_TO_HIGHER_HALF_DATA(
			_acpi_get_rsdp()->xsdt_addr);

		if (!_acpi_verify_sdt(&xsdt->hdr, "XSDT")) {
			log(PANIC, "XSDT was not found\n");
		}
	} else {
		rsdt = (rsdt_t *)PHYS_TO_HIGHER_HALF_DATA(
			_acpi_get_rsdp()->rsdt_addr);

		if (!_acpi_verify_sdt(&rsdt->hdr, "RSDT")) {
			log(PANIC, "RSDT was not found\n");
		}
	}

	_acpi_madt_init();

	log(INFO, "ACPI Initialized\n");
}

sdt_t *_acpi_find_sdt(const char *sig)
{
	size_t entry_count;

	if (_acpi_has_xsdt()) {
		entry_count = (xsdt->hdr.len - sizeof(xsdt->hdr)) / 8;
	} else {
		entry_count = (rsdt->hdr.len - sizeof(rsdt->hdr)) / 4;
	}

	sdt_t *cur_entry;

	for (size_t i = 0; i < entry_count; i++) {
		cur_entry = (sdt_t *)(_acpi_has_xsdt() ? xsdt->entries[i] :
							 rsdt->entries[i]);

		if (_acpi_verify_sdt(cur_entry, sig)) {
			return (sdt_t *)PHYS_TO_HIGHER_HALF_DATA(
				(uintptr_t)cur_entry);
		}
	}

	log(PANIC, "Could not find SDT with sig '%s'!\n", sig);

	return NULL;
}

bool _acpi_verify_sdt(sdt_t *sdt, const char *sig)
{
	return (memcmp(sdt->sig, sig, 4) == 0) &&
	       _acpi_verify_sdt_checksum(sdt, sig);
}

bool _acpi_verify_sdt_checksum(sdt_t *sdt, const char *sig)
{
	uint8_t cs = 0;
	uint8_t *ptr = (uint8_t *)sdt;

	for (uint8_t i = 0; i < sdt->len; i++) {
		cs += ptr[i];
	}

	cs = cs & 0xFF;

	if (cs == 0) {
		log(INFO, "%s's checksum is OK\n", sig);
		return true;
	} else {
		log(PANIC, "Invalid checksum %x for %s!\n", cs, sig);
		return false;
	}
}