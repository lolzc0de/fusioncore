#ifndef __BOOTINFO_H_
#define __BOOTINFO_H_

// TODO: Move this to a more appropriate place
typedef unsigned long long size_t;

typedef struct {
	void *BaseAddr;
	size_t BufSize;
	unsigned int Width;
	unsigned int Height;
	unsigned int PixelsPerScanLine;
} framebuffer_t;

typedef struct {
	framebuffer_t *framebuffer;
	EFI_MEMORY_DESCRIPTOR *mmap;
	UINTN mmap_size;
	UINTN mmap_desc_size;
} boot_info_t;

#endif /* __BOOTINFO_H_ */