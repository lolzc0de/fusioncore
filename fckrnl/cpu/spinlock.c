#include <cpu/spinlock.h>

void spinlock_lock(lock_t lock)
{
	while (__atomic_test_and_set(&lock, __ATOMIC_ACQUIRE));
}

void spinlock_unlock(lock_t lock)
{
	__atomic_clear(&lock, __ATOMIC_RELEASE);
}