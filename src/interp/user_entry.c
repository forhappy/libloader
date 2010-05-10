/* 
 * user_entry.c
 * by WN @ Apr. 26, 2010
 */

#include <common/defs.h>
#include <xasm/processor.h>
#include <xasm/tls.h>
#include <interp/user_entry.h>
#include <interp/checkpoint.h>

/* 
 * it is called from arch's user_branch (compiler.S) -->
 * do_user_branch (user_entry.c)
 *
 * The loader patch the 1st instruction of user's
 * code. When enter into it, the code will jmp to
 * 'user_branch (compiler.S)', and it will be patch back in
 * do_user_branch (user_entry.c).
 *
 * There are also a 'first_branch', which is called before the interpreter, do
 * the patch works.
 *
 * all those functions are called only once. first_branch is called before the
 * execution of ld-linux.so; user_branch-->do_user_branch-->enter_user_code
 * are called before the entry of user's code.
 *
 * after all those works done, do_user_branch (user_entry.c) will set
 * tls->target to user's entry and begin logging.
 */
void
enter_user_code(struct pusha_regs * regs, void * eip)
{
	TRACE(LOADER, "generate the first checkpoint\n");
	fork_make_checkpoint(regs, (eip));
}

// vim:ts=4:sw=4

