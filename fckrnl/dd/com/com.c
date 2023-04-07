#include <asm/utils.h>
#include <dd/com/com.h>

void com_init(com_port_t port)
{
	io_outb(port + 1, 0x00);
	io_outb(port + 3, 0x80);
	io_outb(port + 0, 0x03);
	io_outb(port + 1, 0x00);
	io_outb(port + 3, 0x03);
	io_outb(port + 2, 0xC7);
	io_outb(port + 4, 0x0B);
}

void com_send_char(com_port_t port, char c)
{
	while (!com_is_transmit_empty(port))
		;

	io_outb(port, c);
}

void com_send_string(com_port_t port, char *str)
{
	for (int i = 0; str[i] != '\0'; i++) {
		com_send_char(port, str[i]);
	}
}

void com_recv(com_port_t port)
{
	while (!com_is_received(port))
		;

	return io_inb(port);
}

bool com_is_transmit_empty(com_port_t port)
{
	return io_inb(port + 5) & 0x20;
}

bool com_is_received(com_port_t port)
{
	return io_inb(port + 5) & 1;
}