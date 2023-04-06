#include <efi.h>
#include <efilib.h>
#include <elf.h>

#include "bootinfo.h"

framebuffer_t Framebuffer;

framebuffer_t *InitGOP()
{
	EFI_GUID gopGuid = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;
	EFI_GRAPHICS_OUTPUT_PROTOCOL *GOP;
	EFI_STATUS Status;

	Status = uefi_call_wrapper(BS->LocateProtocol, 3, &gopGuid, NULL, (void**)&GOP);
	if (EFI_ERROR(Status)) {
		Print(L"ERROR: Unable to locate GOP!\r\n");
		return NULL;
	}

	Framebuffer.BaseAddr = (void*)GOP->Mode->FrameBufferBase;
	Framebuffer.BufSize = GOP->Mode->FrameBufferSize;
	Framebuffer.Width = GOP->Mode->Info->HorizontalResolution;
	Framebuffer.Height = GOP->Mode->Info->VerticalResolution;
	Framebuffer.PixelsPerScanLine = GOP->Mode->Info->PixelsPerScanLine;

	return &Framebuffer;
}

EFI_FILE *LoadFile(EFI_FILE *Directory, CHAR16 *Path, EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable)
{
	EFI_FILE *File;
	EFI_LOADED_IMAGE_PROTOCOL *Image;
	EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *FileSystem;
	
	SystemTable->BootServices->HandleProtocol(ImageHandle, &gEfiLoadedImageProtocolGuid, (void**)&Image);
	SystemTable->BootServices->HandleProtocol(Image->DeviceHandle, &gEfiSimpleFileSystemProtocolGuid, (void**)&FileSystem);

	if (Directory == NULL)
		FileSystem->OpenVolume(FileSystem, &Directory);
	
	EFI_STATUS Status = Directory->Open(Directory, &File, Path, EFI_FILE_MODE_READ, EFI_FILE_READ_ONLY);
	if (Status != EFI_SUCCESS) {
		return NULL;
	}

	return File;
}

int MemCmp(const void *a_ptr, const void *b_ptr, size_t n)
{
	const unsigned char *a = a_ptr, *b = b_ptr;
	for (size_t i = 0; i < n; i++) {
		if (a[i] < b[i])
			return -1;
		else if (a[i] > b[i])
			return 1;
	}
	return 0;
}

EFI_STATUS efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable)
{
	InitializeLib(ImageHandle, SystemTable);

	EFI_FILE *Kernel = LoadFile(NULL, L"fckrnl", ImageHandle, SystemTable);
	if (Kernel == NULL) {
		Print(L"Could not load kernel\r\n");
	}

	Elf64_Ehdr Header;
	{
		UINTN FileInfoSize;
		EFI_FILE_INFO *FileInfo;
		Kernel->GetInfo(Kernel, &gEfiFileInfoGuid, &FileInfoSize, NULL);
		SystemTable->BootServices->AllocatePool(EfiLoaderData, FileInfoSize, (void**)&FileInfo);
		Kernel->GetInfo(Kernel, &gEfiFileInfoGuid, &FileInfoSize, (void**)&FileInfo);

		UINTN Size = sizeof(Header);
		Kernel->Read(Kernel, &Size, &Header);
	}

	if (MemCmp(&Header.e_ident[EI_MAG0], ELFMAG, SELFMAG) != 0 ||
		Header.e_ident[EI_CLASS] != ELFCLASS64 ||
		Header.e_ident[EI_DATA] != ELFDATA2LSB ||
		Header.e_type != ET_EXEC ||
		Header.e_machine != EM_X86_64 ||
		Header.e_version != EV_CURRENT) {
		Print(L"Bad Kernel\r\n");
	}

	Elf64_Phdr *Phdrs;
	{
		Kernel->SetPosition(Kernel, Header.e_phoff);
		UINTN Size = Header.e_phnum * Header.e_phentsize;
		SystemTable->BootServices->AllocatePool(EfiLoaderData, Size, (void**)&Phdrs);
		Kernel->Read(Kernel, &Size, Phdrs);
	}

	for (Elf64_Phdr *Phdr = Phdrs; (char *)Phdr < (char *)Phdrs + Header.e_phnum * Header.e_phentsize; Phdr = (Elf64_Phdr *)((char *)Phdr + Header.e_phentsize)) {
		switch (Phdr->p_type) {
		case PT_LOAD:
		{
			int Pages = (Phdr->p_memsz + 0x1000 - 1) / 0x1000;
			Elf64_Addr Segment = Phdr->p_paddr;
			SystemTable->BootServices->AllocatePages(AllocateAddress, EfiLoaderData, Pages, &Segment);
			Kernel->SetPosition(Kernel, Phdr->p_offset);
			UINTN Size = Phdr->p_filesz;
			Kernel->Read(Kernel, &Size, (void *)Segment);
			break;
		}
		}
	}

	framebuffer_t *Fbuf = InitGOP();

	EFI_MEMORY_DESCRIPTOR *Map = NULL;
	UINTN MapSize, MapKey;
	UINTN DescriptorSize;
	UINT32 DescriptorVersion;
	{
		SystemTable->BootServices->GetMemoryMap(&MapSize, Map, &MapKey, &DescriptorSize, &DescriptorVersion);
		SystemTable->BootServices->AllocatePool(EfiLoaderData, MapSize, (void **)&Map);
		SystemTable->BootServices->GetMemoryMap(&MapSize, Map, &MapKey, &DescriptorSize, &DescriptorVersion);
	}

	boot_info_t BootInfo;
	BootInfo.framebuffer = Fbuf;
	BootInfo.mmap = Map;
	BootInfo.mmap_size = MapSize;
	BootInfo.mmap_desc_size = DescriptorSize;

	SystemTable->BootServices->ExitBootServices(ImageHandle, MapKey);

	void (*kmain)(boot_info_t *) = ((__attribute__((sysv_abi)) void(*)(boot_info_t *))Header.e_entry);
	kmain(&BootInfo);

	return EFI_SUCCESS;
}