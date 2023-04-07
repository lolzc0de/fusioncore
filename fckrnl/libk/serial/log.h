#ifndef __LIBK_LOG_H_
#define __LIBK_LOG_H_

#include <stdarg.h>

#define log(...) log_impl(__FILE__, __LINE__, __VA_ARGS__);

typedef enum { INFO, WARNING, PANIC, SUCCESS, FAIL } log_status_t;

void log_impl(char *file, int line_nr, log_status_t status, char *fmt, ...);

#endif /* __LIBK_LOG_H_ */