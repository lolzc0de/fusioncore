#ifndef __PIC_H_
#define __PIC_H_

#include <stdint.h>

#define PIC1			0x20		// master
#define PIC2			0xA0		// slave
#define PIC1_COMMAND	PIC1		// master PIC command port
#define PIC1_DATA		(PIC1+1)	// master PIC data port
#define PIC2_COMMAND	PIC1		// slave PIC command port
#define PIC2_DATA		(PIC1+1)	// slave PIC data port

#define ICW4_8086		0x01		// 8086/88 (MCS-80/86) mode

void pic_disable(void);
void pic_remap(void);
void pic_set_mask(unsigned char irq_line);
void pic_clear_mask(unsigned char irq_line);
void pic_signal_eoi(uint64_t isr_number);

#endif /* __PIC_H_ */