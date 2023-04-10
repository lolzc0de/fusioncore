#ifndef __ACPI_SDT_H_
#define __ACPI_SDT_H_

#include <stdint.h>

typedef struct __attribute__((__packed__)) {
	char sig[4];
	uint32_t len;
	uint8_t rev;
	uint8_t cs;
	char oem_str[6];
	char oem_tbl_id[8];
	uint32_t oem_rev;
	char creat_id[4];
	uint32_t creat_rev;
} sdt_t;

#endif /* __ACPI_SDT_H_ */