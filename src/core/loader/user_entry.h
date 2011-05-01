/* 
 * user_entry.h
 * by WN @ Nov. 08, 2010
 */

#ifndef INTERP_USER_ENTRY_H
#define INTERP_USER_ENTRY_H

#include <common/defs.h>
#include <xasm/processor.h>
#include <interp/arch_user_entry.h>

extern void
patch_user_entry(void);

#if 0
/* don't export this func: needn't */
extern void
unpatch_user_entry(void);
#endif

/* use hidden attribute to generate relocable code */
extern void
arch_user_entry_e(void) ATTR_HIDDEN;

extern void
enter_user_entry(struct pusha_regs * regs, void * pc);

extern void
arch_patch_user_entry(void * uentry, void * rep_start,
		size_t rep_sz, uint32_t prot);

#endif

// vim:ts=4:sw=4

