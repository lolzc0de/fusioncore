#ifndef __CPU_SPINLOCK_H_
#define __CPU_SPINLOCK_H_

#include <stdbool.h>

typedef volatile bool lock_t;

void spinlock_lock(lock_t lock);
void spinlock_unlock(lock_t lock);

#endif /* __CPU_SPINLOCK_H_ */