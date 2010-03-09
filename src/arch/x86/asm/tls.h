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

/* 
 * What's in the tls data section?
 */
struct thread_private_data {
	void * ret_address;
	/* when enter snitchaser's code, stack should be set to it */
	struct thread_private_data * stack_top;	
	/* access: *fs:(0x2000 - 4); stores: the base address of the TLS section */
	void * tls_base;	
};

extern void init_tls(void);

#endif

// vim:ts=4:sw=4

