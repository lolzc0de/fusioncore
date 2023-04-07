#ifndef __REQS_H_
#define __REQS_H_

// these are defined in `main.c`
extern volatile struct limine_kernel_address_request kernel_address_request;
extern volatile struct limine_hhdm_request hhdm_request;
extern volatile struct limine_memmap_request memmap_request;
extern volatile struct limine_stack_size_request stack_size_request;
//extern volatile struct limine_framebuffer_request framebuffer_request;

#endif /* __REQS_H_ */