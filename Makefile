OSNAME = FusionCore

GNUEFI = fcldr/uefi
OVMF = /usr/share/OVMF/x64/OVMF.fd
LDS = fckrnl/x86_64-kernel.ld
CC = x86_64-elf-gcc
LD = x86_64-elf-ld

CFLAGS = -ffreestanding -fshort-wchar -Ifckrnl
LDFLAGS = -T $(LDS) -static -Bsymbolic -nostdlib

SRCDIR := fckrnl
OBJDIR := lib
BUILDDIR := build
BOOTEFI := $(GNUEFI)/x86_64/fcldr/main.efi
KERNEL = $(BUILDDIR)/fckrnl

rwildcard=$(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))

SRC = $(call rwildcard,$(SRCDIR),*.c)
OBJS = $(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(SRC))
DIRS = $(wildcard $(SRCDIR)/*)

.PHONY: bootloader
bootloader:
	make -C fcldr/uefi
	make -C fcldr/uefi fcldr

.PHONY: kernel
kernel: $(KERNEL)

$(KERNEL): $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $^

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $^ -o $@

.PHONY: img
img:
	dd if=/dev/zero of=$(BUILDDIR)/$(OSNAME).img bs=512 count=93750
	mformat -i $(BUILDDIR)/$(OSNAME).img ::
	mmd -i $(BUILDDIR)/$(OSNAME).img ::/EFI
	mmd -i $(BUILDDIR)/$(OSNAME).img ::/EFI/BOOT
	mcopy -i $(BUILDDIR)/$(OSNAME).img $(KERNEL) ::
	mcopy -i $(BUILDDIR)/$(OSNAME).img $(BOOTEFI) ::/EFI/BOOT/BOOTX64.EFI
	mcopy -i $(BUILDDIR)/$(OSNAME).img startup.nsh ::

.PHONY: run
run:
	qemu-system-x86_64 -drive format=raw,file=$(BUILDDIR)/$(OSNAME).img -m 256M -cpu qemu64 -bios $(OVMF) -net none