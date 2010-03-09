/* 
 * common/spinlock.h
 * by WN @ Mar. 09, 2010
 */

#ifndef __COMMON_SPINLOCK_H
#define __COMMON_SPINLOCK_H

#include <config.h>
#include <common/defs.h>
#include <common/assert.h>
#include <asm/raw_spinlock.h>
#include <asm/syscall.h>

struct spinlock_t {
	struct raw_spinlock_t raw_lock;
};

#define SPINLOCK_UNLOCKED { .raw_lock = RAW_SPINLOCK_UNLOCKED }

#define DEF_SPINLOCK_UNLOCKED(name)	struct spinlock_t name = SPINLOCK_UNLOCKED
#define INIT_SPINLOCK_UNLOCKED(l)	do { INIT_RAW_SPINLOCK_UNLOCKED(&(l->raw_lock)); } while(0);

static __AI void
spin_lock(struct spinlock_t * lock)
{
	for (;;) {
		if (raw_spin_trylock(&lock->raw_lock))
			break;
		INTERNAL_SYSCALL_int80(sched_yield, 0);
	}
}

static __AI void
spin_unlock(struct spinlock_t * lock)
{
	assert(raw_spin_is_locked_by_value(
				raw_spin_unlock(&lock->raw_lock)));
}

#endif

// vim:ts=4:sw=4

