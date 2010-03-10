/* 
 * tls.h
 * by WN @ Mar. 09, 2010
 */

#ifndef __X86_TLS_H
#define __X86_TLS_H

#include <config.h>
#include <common/defs.h>
#include <common/debug.h>
#include <common/assert.h>
#include <asm/syscall.h>

/* thread thread stack: from 0x4000, each for 2 pages(8k)
 * memory is protected by ldt
 * */
/* x86 have at most 8192 ldt entry, so at most 8192 tls.
 * each thread's tls section is predefined:
 * each thread has 2 pages for stack, the thread self's info
 * is located at the bottom of the stack.
 * if we have 8192 (0x2000) threads, the tls stack is from 0x4000 to 0x4004000;
 * normal program always put its ode section at 0x8048000, so
 * the stack never clobber code section.
 */


/* 
 * What's in the tls data section?
 */
struct thread_private_data {
	void * ret_address;
	/* we also need some code cache stuff */
	/* when enter snitchaser's code, stack should be set to it */
	struct thread_private_data * stack_top;	
	/* access: *fs:(0x2000 - 4); stores: the base address of the TLS section */
	void * tls_base;	
};

#define TLS_SIZE			(0x2000)
#define MAX_TLS_ADDR		(TLS_SIZE)
#define PRIVATE_DATA_ADDR	(0x2000-sizeof(struct thread_private_data))


extern void init_tls(void);

static __AI void *
get_tls_base(void)
{
	void * r;
	asm volatile (
			"movl %%fs:%[b_addr], %[o]\n"
			: [o] "=rm" (r)
			: [b_addr] "i" (PRIVATE_DATA_ADDR + offsetof(struct thread_private_data, tls_base))
			);
	return r;
}

#endif

// vim:ts=4:sw=4

