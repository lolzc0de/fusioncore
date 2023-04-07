#include <dd/com/com.h>
#include <libk/serial/debug.h>
#include <libk/kprintf/kprintf.h>

const char dbg_buf[5120];

void debug(char *fmt, ...)
{
	va_list ptr;
	va_start(ptr, fmt);
	vsnprintf((char *)&dbg_buf, -1, fmt, ptr);

	com_send_string(COM1, (char *)dbg_buf);

	va_end(ptr);
}

void debug_set_col(char *col_code)
{
	if (col_code[0] != '\e' || col_code[1] != '[')
		return;

	com_send_string(COM1, col_code);
}