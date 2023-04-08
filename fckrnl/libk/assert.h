#ifndef __ASSERT_H_
#define __ASSERT_H_

#include <stdbool.h>

#define assert(cond) __assert(__FILE__, __LINE__, cond, #cond)

void __assert(char *file, int line_nr, bool cond, char *cond_str);

#endif /* __ASSERT_H_ */