#include <boot/stivale2.h>

#include <acpi/acpi.h>
#include <acpi/tbl/madt.h>

#include <libk/malloc.h>
#include <libk/serial/log.h>

madt_t *madt;

madt_lapic_t **madt_lapics;
madt_ioapic_t **madt_ioapics;
madt_iso_t **madt_isos;
madt_lapic_nmi_t **madt_lapic_nmis;

size_t madt_lapics_i = 0;
size_t madt_ioapics_i = 0;
size_t madt_isos_i = 0;
size_t madt_lapic_nmis_i = 0;

void _acpi_madt_init(void)
{
	madt = (madt_t *)(uintptr_t)_acpi_find_sdt("APIC");

	if (madt == NULL) {
		log(PANIC, "MADT wasn't found!\n");
	}

	madt_lapics = malloc(256);
	madt_ioapics = malloc(256);
	madt_isos = malloc(256);
	madt_lapic_nmis = malloc(256);

	uint8_t *start = (uint8_t *)&madt->entries;
	size_t end = (size_t)&madt->hdr + madt->hdr.len;

	for (uint8_t *entry_ptr = start; (size_t)entry_ptr < end;
	     entry_ptr += *(entry_ptr + 1)) {
		switch (*entry_ptr) {
		case 0:
			log(INFO, "_acpi_madt_init(): Found LAPIC at 0x%llx\n",
			    entry_ptr);
			madt_lapics[madt_lapics_i++] =
				(madt_lapic_t *)entry_ptr;
			break;
		case 1:
			log(INFO, "_acpi_madt_init(): Found IOAPIC at 0x%llx\n",
			    entry_ptr);
			madt_ioapics[madt_ioapics_i++] =
				(madt_ioapic_t *)entry_ptr;
			break;
		case 2:
			log(INFO,
			    "_acpi_madt_init(): Found Interrupt Source Override at 0x%llx\n",
			    entry_ptr);
			madt_isos[madt_isos_i++] = (madt_iso_t *)entry_ptr;
			break;
		case 4:
			log(INFO,
			    "_acpi_madt_init(): Found Non Maskable Interrupt at 0x%llx\n",
			    entry_ptr);
			madt_lapic_nmis[madt_lapic_nmis_i++] =
				(madt_lapic_nmi_t *)entry_ptr;
			break;
		}
	}

	log(INFO, "MADT Initialized\n");
}