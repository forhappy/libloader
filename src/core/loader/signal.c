/* 
 * signal.c
 * by WN @ Oct. 24, 2010
 */

#include <asm/signal.h>
#include <defs.h>
#include <debug.h>
#include <loader/snitchaser_tpd.h>
#include <signal.h>
#include <asm_syscall.h>
#include <syscall.h>
#include <mutex.h>


sigset_t
block_signals(void)
{
	int err;
	sigset_t in, out;

	set_maskall_sigset(&in);
	err = INTERNAL_SYSCALL_int80(rt_sigprocmask, 4,
			SIG_SETMASK, &in, &out, sizeof(in));
	assert(err == 0);
	return out;
}

void
restore_signals(sigset_t mask)
{
	int err;
	err = INTERNAL_SYSCALL_int80(rt_sigprocmask, 4,
			SIG_SETMASK, &mask, NULL, sizeof(mask));
	assert(err == 0);
}

DEFINE_UNLOCKED_XMUTEX(user_sigactions_mutex); 
struct sigaction user_sigactions[_NSIG + 1];

void
restore_sigactions(void)
{
	int err;
	for (int i = 1; i < _NSIG; i++) {
		if ((i == SIGKILL) || (i == SIGSTOP))
			continue;
		err = sys_rt_sigaction(i, &user_sigactions[i], NULL,
				sizeof(sigset_t));
		assert(err == 0);
	}
	return;
}



// vim:ts=4:sw=4

