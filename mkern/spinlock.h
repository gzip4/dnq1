#ifndef MKERN_SPINLOCK_H
#define MKERN_SPINLOCK_H

#include <config.h>

typedef volatile int spinlock_t;

static inline void pause()
{
	__asm__ __volatile__ ("pause" ::: "memory");
}

static inline void spinlock_accuire(spinlock_t &lck)
{
#if defined(CONFIG_SMP)
	while (__sync_lock_test_and_set(&lck, 1))
		while(lck)
			pause();
#endif
}

static inline void spinlock_release(spinlock_t &lck)
{
#if defined(CONFIG_SMP)
	__sync_lock_release(&lck);
#endif
}

#endif /* MKERN_SPINLOCK_H */
