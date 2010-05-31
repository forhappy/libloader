/* 
 * syscall_handler.h
 * by WN @ May. 31, 2010
 */

#ifndef SYSCALL_HANDLER_H
#define SYSCALL_HANDLER_H

#if 0
#include <common/defs.h>
#include <interp/logger.h>
#include <xasm/processor.h>
#endif

#include <linux/types.h>

/* 
 * include as less files as possible, because we need  kernel structures
 */

extern void
append_buffer(void * data, size_t size);

extern void
load_from_buffer(void * data, size_t size);

#ifndef ATTR_UNUSED
# define ATTR_UNUSED __attribute__((unused))
#endif

struct pusha_regs {
	uint32_t flags;
	uint32_t edi;
	uint32_t esi;
	uint32_t ebp;
	uint32_t esp;
	uint32_t ebx;
	uint32_t edx;
	uint32_t ecx;
	uint32_t eax;
};

#ifdef POST_LIBRARY
# define DEF_HANDLER(name)	int post_##name(struct pusha_regs * regs ATTR_UNUSED)
# define INT_VAL(x)		({int ___x = (x); append_buffer(&___x, sizeof(int)); ___x;})
# define BUFFER(p, s)	do {append_buffer((p), (s));} while(0)
#endif

#ifdef REPLAY_LIBRARY
# define DEF_HANDLER(name)	int replay_##name(struct pusha_regs * regs ATTR_UNUSED)
# define INT_VAL(x)		({load_from_buffer(&(x), sizeof(int)); (x);})
# define BUFFER(p, s)	do {load_from_buffer((p), (s));} while(0)
#endif

#define EAX_AS_INT		(INT_VAL(regs->eax))

#endif


// vim:ts=4:sw=4

