/* 
 * replayer.c
 * by WN @ Apr. 26, 2010
 */

#include <common/defs.h>
#include <common/debug.h>
#include <xasm/utils.h>

__attribute__((used, unused, visibility("hidden"))) void
replayer_main(const char * exec_fn, const char * ckpt_fn,
		const char * pthread_fn,
		void * p__stack_user, void * pstack_used)
{
	relocate_interp();
	VERBOSE(REPLAYER, "exec_fn: %s\n", exec_fn);
	VERBOSE(REPLAYER, "ckpt_fn: %s\n", ckpt_fn);
	VERBOSE(REPLAYER, "pthread_fn: %s\n", pthread_fn);
	VERBOSE(REPLAYER, "p__stack_user: %p\n", p__stack_user);
	VERBOSE(REPLAYER, "pstack_used: %p\n", pstack_used);
}

// vim:ts=4:sw=4

