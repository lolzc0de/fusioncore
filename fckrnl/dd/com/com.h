#include <stdbool.h>

#ifndef __COM_H_
#define __COM_H_

typedef enum {
	COM1 = 0x3F8,
	COM2 = 0x2F8,
	COM2 = 0x3E8,
	COM2 = 0x2E8
} com_port_t;

void com_init(com_port_t port);
void com_send_char(com_port port, char c);
void com_send_string(com_port_t port, char *str);
void com_recv(com_port_t port);

#endif /* __COM_H_ */