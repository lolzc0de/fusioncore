#include <stdint.h>

#ifndef __UTILS_H_
#define __UTILS_H_

void io_outb(uint16_t port, uint8_t value);
uint8_t io_inb(uint16_t port);
void io_wait(void);

#endif /* __UTILS_H_ */