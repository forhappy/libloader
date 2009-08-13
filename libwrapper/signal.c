/* FIXME check for tracing... */

/* 
 * signal.c
 * by WN @ Aug. 14, 2009
 */

#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>

#include "injector.h"
#include "checkpoint/checkpoint.h"
#include "injector_utils.h"
#include "injector_debug.h"

#include "signal_defs.h"

extern SCOPE char logger_filename[128];
extern SCOPE char ckpt_filename[128];
extern SCOPE pid_t self_pid;

/* this var is inited each time we enter a syscall */
extern const struct syscall_regs * signal_regs;

extern void wrapped_rt_sigreturn(void);
extern void wrapped_sigreturn(void);
extern void wrapped_sighandler(void);

void SCOPE
unhook_sighandlers(void)
{
	struct k_sigaction * acts = state_vector.sigactions;
	/* for each signal, we remove the SA_RESTORER flag and reset it */
	for (int i = 0; i < K_NSIG; i++) {
		int err;
		struct k_sigaction act;
		err = INTERNAL_SYSCALL(rt_sigaction, 4,
				i, NULL, &act, sizeof(act));
		ASSERT(err != 0, NULL, "rt_sigaction %d failed\n", i);
		if (act.sa_handler == wrapped_sighandler) {
			act.sa_handler = acts[i].sa_handler;
			err = INTERNAL_SYSCALL(rt_sigaction, 4,
					i, &act, NULL, sizeof(act));
			ASSERT(err != 0, NULL, "rt_sigaction %d failed\n", i);
		}
	}
}


#define SA_SIGINFO	0x00000004u
void
do_wrapped_sighandler(volatile struct syscall_regs handler_regs,
		volatile uint32_t real_sighandler,
		volatile uint32_t pretcode,
		volatile int signum)
{
	INJ_WARNING("proc received signal %d\n", signum);
	struct k_sigaction * pact = &state_vector.sigactions[signum];
	real_sighandler = (uint32_t)(pact->sa_handler);

	if (pact->sa_flags & SA_SIGINFO)
		pretcode = (uint32_t)(wrapped_rt_sigreturn);
	else
		pretcode = (uint32_t)(wrapped_sigreturn);

	/* now, procmask has changes. */
	/* we should restore this state in sigreturn. then when following
	 * syscall DISABLE and ENABLE signal, mask works correctly. */
	/* we write this before we make ckpt. */
	k_sigset_t mask = pact->sa_mask;
	k_sigset_t * pm = &state_vector.sigmask;
	sigaddset(&mask, signum);
	sigorsets(pm, pm, &mask);

	handler_regs.esp += 4;

	/* check whether inside a syscall */
	if (IS_BREAK_SYSCALL()) {
		INJ_WARNING("syscall %d is breaked by this signal\n",
				signal_regs->orig_eax);
	} else {
		INJ_WARNING("no syscall is broken\n");
	}

	/* finally, we unlock signals */
	/* notice: we need to block this signal itself */
	/* reset sigmask to correct one */
	INTERNAL_SYSCALL(rt_sigprocmask, 4,
		SIG_SETMASK, pm, NULL, sizeof(k_sigset_t));
}

void SCOPE
do_wrapped_sigreturn(struct sigframe frame)
{
	INJ_WARNING("here!\n");
	/* we restore sigprocmask */
	k_sigset_t mask;
	memcpy(&mask.sig[0], &frame.sc.oldmask, sizeof(frame.sc.oldmask));
	memcpy(&mask.sig[1], &frame.extramask, sizeof(frame.extramask));
	state_vector.sigmask = mask;
}

void SCOPE
do_wrapped_rt_sigreturn(struct rt_sigframe frame)
{
	INJ_WARNING("here!\n");
	state_vector.sigmask = frame.uc.uc_sigmask;
}

