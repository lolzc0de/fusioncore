SRCS += $(shell find . -name '*.c')
OBJS := $(SRCS:%.c=build/%.o)
OBJDIRS := $(dir $(OBJS))

INCLUDE_DIRS += lib/
INCLUDE_DIRS += lib/libc
INCLUDE_DIRS += src

CFLAGS +=	-target x86_64-unknown-windows \
			-ffreestanding \
			-fno-stack-check \
			-fno-stack-protector \
			-fshort-wchar \
			-mno-red-zone

CFLAGS += $(INCLUDE_DIRS:%=-I%)

LDFLAGS +=	-target x86_64-unknown-windows \
			-nostdlib \
			-Wl,-entry:EfiMain \
			-Wl,-subsystem:efi_application \
			-fuse-ld=lld-link

.PHONY: all clean BOOTX64.EFI BOOTX64.DLL

all: BOOTX64.EFI

BOOTX64.EFI: bin/BOOTX64.EFI

bin/BOOTX64.EFI: $(OBJDIRS) $(OBJS)
	mkdir -p bin
	clang $(LDFLAGS) -o bin/BOOTX64.EFI $(OBJS)

build/%.o: %.c
	clang $(CFLAGS) -c -o $@ $<

build/%:
	mkdir -p $@

clean:
	rm -rf build bin image