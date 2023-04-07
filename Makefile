OSNAME = FusionCore

GNUEFI = fcldr/uefi
OVMF = /usr/share/OVMF/x64/OVMF.fd
LDS = fckrnl/x86_64-kernel.ld
CC = x86_64-elf-gcc
LD = x86_64-elf-ld

CFLAGS = -std=gnu11 \
    -ffreestanding \
    -fno-stack-protector \
    -fno-stack-check \
    -fno-lto \
    -fno-pie \
    -fno-pic \
    -m64 \
    -march=x86-64 \
    -mabi=sysv \
    -mno-80387 \
    -mno-mmx \
    -mno-sse \
    -mno-sse2 \
    -mno-red-zone \
    -mcmodel=kernel \
	-Ifckrnl -Icore/inc

LDFLAGS = -nostdlib \
    -static \
    -m elf_x86_64 \
    -z max-page-size=0x1000 \
    -T $(LDS)

KERN_DIR := fckrnl
CORE_DIR := core
OBJDIR := lib
BUILDDIR := build
BOOTEFI := $(GNUEFI)/x86_64/fcldr/main.efi
KERNEL = $(BUILDDIR)/fckrnl

rwildcard=$(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))

KERN_SRC = $(call rwildcard,$(KERN_DIR),*.c)
CORE_SRC = $(call rwildcard,$(CORE_DIR),*.c)

KERN_OBJ = $(patsubst $(KERN_DIR)/%.c, $(OBJDIR)/%.o, $(KERN_SRC))
CORE_OBJ = $(patsubst $(CORE_DIR)/%.c, $(OBJDIR)/%.o, $(CORE_SRC))

OBJS = $(KERN_OBJ) $(CORE_OBJ)
DIRS = $(wildcard $(KERN_DIR)/*) $(wildcard $(CORE_DIR)/*)

.PHONY: all
all: limine kernel iso

.PHONY: limine
limine:
	git clone https://github.com/limine-bootloader/limine --branch=v4.x-branch-binary limine/
	make -C limine/

.PHONY: kernel
kernel: $(KERNEL)

$(KERNEL): $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $^

$(OBJDIR)/%.o: $(KERN_DIR)/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $^ -o $@

$(OBJDIR)/%.o: $(CORE_DIR)/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $^ -o $@

.PHONY: iso
iso:
	mkdir iso
	cp $(KERNEL) limine.cfg limine/limine.sys limine/limine-cd.bin limine/limine-cd-efi.bin iso/
	xorriso -as mkisofs -b limine-cd.bin \
        -no-emul-boot -boot-load-size 4 -boot-info-table \
        --efi-boot limine-cd-efi.bin \
        -efi-boot-part --efi-boot-image --protective-msdos-label \
        iso -o fusioncore.iso
	limine/limine-deploy fusioncore.iso
	rm -rf iso/

.PHONY: run
run:
	qemu-system-x86_64 -cdrom fusioncore.iso -m 256M -cpu qemu64 -bios $(OVMF) -net none

.PHONY: clean
clean:
	rm -rf lib iso build fusioncore.iso

.PHONY: distclean
distclean: clean
	rm -rf limine/