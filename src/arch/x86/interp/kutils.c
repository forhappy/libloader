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

void
block_signals(void)
{
	memset(&all_mask, 0xff, sizeof(all_mask));
	INTERNAL_SYSCALL_int80(rt_sigprocmask, 4,
			SIG_SETMASK, &all_mask, &old_mask, sizeof(all_mask));
	block_level ++;
}

void
restore_signals(void)
{
	block_level --;
	if (block_level == 0) {
		INTERNAL_SYSCALL_int80(rt_sigprocmask, 4,
				SIG_SETMASK, &old_mask, NULL, sizeof(all_mask));
	}
}

// vim:ts=4:sw=4

