/* 
 * user_entry.h
 * by WN @ Apr. 26, 2010
 */

#include <common/defs.h>
#include <xasm/processor.h>

extern ATTR_HIDDEN void
first_branch(void * user_entry, void * interp_entry);

extern ATTR_HIDDEN void
enter_user_code(struct pusha_regs * regs, void * addr);


// vim:ts=4:sw=4

