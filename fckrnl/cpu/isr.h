#ifndef __ISR_H_
#define __ISR_H_

#define SYSCALL_INT 128
#define SPURIOUS_INT 255

uint64_t isr_handler(uint64_t rsp);

#endif /* __ISR_H_ */