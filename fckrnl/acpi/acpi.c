#include <boot/stivale2.h>
#include <boot/boot.h>

#include <acpi/acpi.h>
#include <acpi/tbl/sdt.h>
#include <acpi/tbl/rsdt.h>

void acpi_init(struct stivale2_struct *stivale2_struct)
{
	struct stivale2_struct_tag_rsdp *rsdp_tag =
		stivale2_get_tag(stivale2_struct, STIVALE2_STRUCT_TAG_RSDP_ID);

	_acpi_rsdp_init(rsdp_tag->rsdp);
}