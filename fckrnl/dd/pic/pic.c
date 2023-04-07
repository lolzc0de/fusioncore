#include <asm/utils.h>
#include <dd/pic/pic.h>

void pic_disable(void)
{
	io_outb(PIC2_DATA, 0xFF);
	io_outb(PIC1_DATA, 0xFF);
}

void pic_remap(void)
{
	uint8_t mask1 = io_inb(PIC1_DATA);
	uint8_t mask2 = io_inb(PIC2_DATA);

	io_outb(PIC1_COMMAND, 0x11);
	io_outb(PIC2_COMMAND, 0x11);
	io_wait();

	io_outb(PIC1_DATA, 0x20);
	io_outb(PIC2_DATA, 0x28);
	io_wait();

	io_outb(PIC1_DATA, 0x04);
	io_outb(PIC2_DATA, 0x02);
	io_wait();

	io_outb(PIC1_DATA, 0x01);
	io_outb(PIC2_DATA, 0x01);
	io_wait();

	io_outb(PIC1_DATA, 0x00);
	io_outb(PIC2_DATA, 0x00);
	io_wait();

	io_outb(PIC1_DATA, ICW4_8086);
	io_wait();
	io_outb(PIC2_DATA, ICW4_8086);
	io_wait();

	io_outb(PIC1_DATA, mask1);
	io_outb(PIC2_DATA, mask2);
}

void pic_set_mask(uint8_t irq_line)
{
	uint16_t port;
	uint8_t value;

	if (irq_line < 8) {
		port = PIC1_DATA;
	} else {
		port = PIC2_DATA;
		irq_line -= 8;
	}

	value = io_inb(port) | (1 << irq_line);
	io_outb(port, value);
}

void pic_clear_mask(uint8_t irq_line)
{
    uint16_t port;
    uint8_t value;

    if (irq_line < 8) {
        port = PIC1_DATA;
	} else {
        port = PIC2_DATA;
        irq_line -= 8;
    }

    value = io_inb(port) & ~(1 << irq_line);
    io_outb(port, value);
}

void pic_signal_eoi(uint64_t isr_number)
{
	// check if IRQ comes from slave PIC
	if (isr_number >= 40)
		io_outb(PIC2_COMMAND, 0x20);
	
	// IRQ comes from master (and from slave) PIC
	io_outb(PIC1_COMMAND, 0x20);
}