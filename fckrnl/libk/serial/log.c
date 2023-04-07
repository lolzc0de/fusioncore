#include <libk/kprintf/kprintf.h>
#include <libk/serial/debug.h>
#include <libk/serial/log.h>

const char log_buf[5120];

void log_impl(char *file, int line_nr, log_status_t status, char *fmt, ...)
{
	va_list ptr;
	va_start(ptr, fmt);
	vsnprintf((char *)&log_buf, -1, fmt, ptr);

	if (status == INFO) {
		debug_set_col(TERM_CYAN);
		debug("[INFO]    | %s:%d -> %s", file, line_nr,
		      (char *)log_buf);
	} else if (status == WARNING) {
		debug_set_col(TERM_YELLOW);
		debug("[WARNING] | %s:%d -> %s", file, line_nr,
		      (char *)log_buf);
	} else if (status == PANIC) {
		debug_set_col(TERM_RED);
		debug("[PANIC]   | %s:%d -> %s", file, line_nr,
		      (char *)log_buf);

		for (;;)
			asm volatile("cli; hlt");
	} else if (status == SUCCESS) {
		debug_set_col(TERM_GREEN);
		debug("[ * ]	| %s:%d -> %s", file, line_nr, (char *)log_buf);
	} else if (status == FAIL) {
		debug_set_col(TERM_PURPLE);
		debug("[ ! ]	| %s:%d -> %s", file, line_nr, (char *)log_buf);
	}

	debug_set_col(TERM_COLOR_RESET);
}