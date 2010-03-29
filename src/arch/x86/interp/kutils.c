/* 
 * kutils.c
 * by WN @ Mar. 22, 2010
 * utils which related to kernel stuff
 */

#include <linux/signal.h>
#include <xasm/kutils.h>
#include <xasm/syscall.h>
#include <xasm/unistd.h>

static sigset_t all_mask = {
	.sig = {0xffffffff, 0xffffffff},
};

static sigset_t old_mask;
static int block_level = 0;

#ifdef block_signals
# undef block_signals
#endif

void
block_signals(void * save_sigmask, int * block_level)
{
	if (*block_level == 0) {
		INTERNAL_SYSCALL_int80(rt_sigprocmask, 4,
				SIG_SETMASK, &all_mask, save_sigmask, sizeof(all_mask));
	}
	*block_level ++;
}

#ifdef restore_signals
# undef restore_signals
#endif
void
restore_signals(void * save_sigmask, int * block_level)
{
	*block_level --;
	if (*block_level <= 0) {
		*block_level = 0;
		INTERNAL_SYSCALL_int80(rt_sigprocmask, 4,
				SIG_SETMASK, save_sigmask, NULL, sizeof(all_mask));
	}
}

// vim:ts=4:sw=4

