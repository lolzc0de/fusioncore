#include <libk/serial/log.h>
#include <libk/assert.h>

void __assert(char *file, int line_nr, bool cond, char *cond_str)
{
	if (!cond) {
		log(PANIC, "Assert failed at: %s:%d with %s as the condition\n", file, line_nr, cond_str);
	}
}