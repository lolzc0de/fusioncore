#ifndef __UTILS_H_
#define __UTILS_H_

#include <stdint.h>

#define write_cr(reg, val) ({ asm volatile("mov %0, %%cr" #reg ::"r"(val)); })

#define read_cr(reg)                                              \
	({                                                        \
		uint64_t val = 0;                                 \
		asm volatile("mov %%cr" #reg ", %0" : "=r"(val)); \
		val;                                              \
	})

static inline void io_outb(uint16_t port, uint8_t val)
{
	asm volatile("outb %0, %1" : : "a"(val), "Nd"(port));
}

static inline uint8_t io_inb(uint16_t port)
{
	uint8_t ret;
	asm volatile("inb %1, %0" : "=a"(ret) : "Nd"(port));
	return ret;
}

static inline void io_wait(void)
{
	io_inb(0x80);
}

static inline void invlpg(uint64_t *address)
{
	asm volatile("invlpg (%0)" : : "r"(address));
}

#endif /* __UTILS_H_ */