
#include "syscalls.h"


#define SIG_BLOCK          0	/* for blocking signals */
#define SIG_UNBLOCK        1	/* for unblocking signals */
#define SIG_SETMASK        2	/* for setting the signal mask */


#define _SIG_SET_BINOP(name, op)					\
static inline void name(k_sigset_t *r, const k_sigset_t *a, const k_sigset_t *b) \
{									\
	extern void _NSIG_WORDS_is_unsupported_size(void);		\
	unsigned long a0, a1, a2, a3, b0, b1, b2, b3;			\
									\
	switch (_NSIG_WORDS) {						\
	    case 4:							\
		a3 = a->sig[3]; a2 = a->sig[2];				\
		b3 = b->sig[3]; b2 = b->sig[2];				\
		r->sig[3] = op(a3, b3);					\
		r->sig[2] = op(a2, b2);					\
	    case 2:							\
		a1 = a->sig[1]; b1 = b->sig[1];				\
		r->sig[1] = op(a1, b1);					\
	    case 1:							\
		a0 = a->sig[0]; b0 = b->sig[0];				\
		r->sig[0] = op(a0, b0);					\
		break;							\
	    default:							\
		_NSIG_WORDS_is_unsupported_size();			\
	}								\
}

#define _sig_or(x,y)	((x) | (y))
_SIG_SET_BINOP(sigorsets, _sig_or)
#define _sig_and(x,y)	((x) & (y))
_SIG_SET_BINOP(sigandsets, _sig_and)
#define _sig_nand(x,y)	((x) & ~(y))
_SIG_SET_BINOP(signandsets, _sig_nand)

#ifndef SIGSTOP
# define SIGSTOP		(19)
#endif

#ifndef SIGKILL
# define SIGKILL		(9)
#endif


static inline void sigdelsetmask(k_sigset_t *set, unsigned long mask)
{
	set->sig[0] &= ~mask;
}

#define sigmask(sig)	(1UL << ((sig) - 1))

int SCOPE
post_rt_sigprocmask(struct syscall_regs * regs)
{
	write_syscall_nr(__NR_sigprocmask);
	write_eax(regs);
	if (regs->eax == 0) {
		if (regs->edx != 0) 
			write_mem(regs->edx, regs->esi);
		int how = regs->ebx;
		k_sigset_t mask;
		__dup_mem(&mask, regs->ecx, regs->esi);

		sigdelsetmask(&mask, sigmask(SIGKILL)|sigmask(SIGSTOP));

		if (how == SIG_BLOCK) {
			sigorsets(&state_vector.sigmask,
					&state_vector.sigmask,
					&mask);
		} else if (how == SIG_UNBLOCK) {
			signandsets(&state_vector.sigmask,
					&state_vector.sigmask,
					&mask);
		} else {
			/* SIG_SETMASK */
			state_vector.sigmask = mask;
		}
	}
	return 0;
}

