#ifndef __ACPI_H_
#define __ACPI_H_

#include <boot/stivale2.h>

#include <acpi/tbl/sdt.h>

#include <stdbool.h>

void acpi_init(struct stivale2_struct *stivale2_struct);

sdt_t *_acpi_find_sdt(const char *sig);
bool _acpi_verify_sdt(sdt_t *sdt, const char *sig);
bool _acpi_verify_sdt_checksum(sdt_t *sdt, const char *sig);

#endif /* __ACPI_H_ */