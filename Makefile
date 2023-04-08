TARGET		:= fckrnl/fckrnl
ISO_IMAGE	:= fusioncore.iso

ARCH = @x86_64-elf

CC	= $(ARCH)-gcc
AS	= @nasm
LD	= $(ARCH)-ld

CC_FLAGS	= -Wall -Wextra -pipe
AS_FLAGS	= -felf64
LD_FLAGS	=

INTERNAL_LD_FLAGS :=		\
	-Tfckrnl/linker.ld	\
	-nostdlib		\
	-static			\
	-zmax-page-size=0x1000	\
	--no-dynamic-linker	\
	-ztext					

INTERNAL_CC_FLAGS :=		\
 	-Ifckrnl		\
	-std=gnu11		\
	-ffreestanding		\
	-fno-stack-protector	\
	-fno-pic -fpie		\
	-mno-80387		\
	-mno-mmx		\
	-mno-3dnow		\
	-mno-sse		\
	-mno-sse2		\
	-mno-red-zone

C_FILES		:= $(shell find fckrnl/ -type f -name '*.c')
AS_FILES	:= $(shell find fckrnl/ -type f -name '*.s')

C_OBJ	= $(C_FILES:.c=.o)
AS_OBJ	= $(AS_FILES:.s=.o)
OBJ	= $(C_OBJ) $(AS_OBJ)

.PHONY: all all_dbg clean format run run_dbg

all: CC_FLAGS += -O3
all: $(TARGET)

all_dbg: CC_FLAGS += -O0
all_dbg: CC_FLAGS += -ggdb
all_dbg: $(TARGET)

run: $(ISO_IMAGE)
	qemu-system-x86_64 -m 2G -serial stdio -cdrom $(ISO_IMAGE)

run_dbg: $(ISO_IMAGE)
	qemu-system-x86_64 -M q35 -m 2G -serial stdio -cdrom $(ISO_IMAGE) -s -S

limine:
	make -C third_party/limine

$(TARGET): $(OBJ)
	$(LD) $(OBJ) $(LD_FLAGS) $(INTERNAL_LD_FLAGS) -o $@


$(ISO_IMAGE): limine $(TARGET)
	rm -rf iso_root
	mkdir -p iso_root
	cp $(TARGET) 								\
		limine.cfg third_party/limine/limine.sys			\
		third_party/limine/limine-cd.bin 				\
		third_party/limine/limine-eltorito-efi.bin iso_root/
	xorriso -as mkisofs -b limine-cd.bin					\
		-no-emul-boot -boot-load-size 4 -boot-info-table		\
		--efi-boot limine-eltorito-efi.bin				\
		-efi-boot-part --efi-boot-image --protective-msdos-label	\
		iso_root -o $(ISO_IMAGE)
	third_party/limine/limine-install $(ISO_IMAGE)
	rm -rf iso_root

%.o: %.c
	@printf " [CC]\t$<\n";
	$(CC) $(CC_FLAGS) $(INTERNAL_CC_FLAGS) -c $< -o $@

%.o: %.s
	@printf " [AS]\t$<\n";
	$(AS) $(AS_FLAGS) $< -o $@

clean:
	rm -rf $(TARGET) $(OBJ) $(ISO_IMAGE)

format:
	find fckrnl -type f -iname "*.c" -o -iname "*.h" | xargs clang-format -i
