#include <stdbool.h>

#ifndef __COM_H_
#define __COM_H_

typedef enum {
	COM1 = 0x3F8,
	COM2 = 0x2F8,
	COM3 = 0x3E8,
	COM4 = 0x2E8
} com_port_t;

void com_init(com_port_t port);
void com_send_char(com_port_t port, char c);
void com_send_string(com_port_t port, char *str);
char com_recv(com_port_t port);

///
bool com_is_transmit_empty(com_port_t port);
bool com_is_received(com_port_t port);

#endif /* __COM_H_ */