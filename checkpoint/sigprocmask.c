
#include "syscalls.h"

#ifndef SYSCALL_PRINTER

#define SIG_BLOCK          0	/* for blocking signals */
#define SIG_UNBLOCK        1	/* for unblocking signals */
#define SIG_SETMASK        2	/* for setting the signal mask */
#ifndef SIGSTOP
# define SIGSTOP		(19)
#endif

#ifndef SIGKILL
# define SIGKILL		(9)
#endif
#define sigmask(sig)	(1UL << ((sig) - 1))

static inline void sigaddsetmask(k_sigset_t *set, unsigned long mask)
{
	set->sig[0] |= mask;
}

static inline void sigdelsetmask(k_sigset_t *set, unsigned long mask)
{
	set->sig[0] &= ~mask;
}

int SCOPE
post_sigprocmask(const struct syscall_regs * regs)
{
	write_eax(regs);
	if (regs->eax == 0) {
		int how = regs->ebx;
		uintptr_t set = regs->ecx;
		uintptr_t oset = regs->edx;
		write_obj(oset);
		if (set) {
			uint32_t new_set;
			__dup_mem(&new_set, set, sizeof(new_set));
			new_set &= ~(sigmask(SIGKILL) | sigmask(SIGSTOP));

			switch (how) {
				case SIG_BLOCK:
					sigaddsetmask(&state_vector.sigmask,
							new_set);
					break;
				case SIG_UNBLOCK:
					sigdelsetmask(&state_vector.sigmask,
							new_set);
					break;
				case SIG_SETMASK:
					state_vector.sigmask.sig[0] = new_set;
					break;
				default:
					INJ_WARNING("$%!@#\n");
					__exit(-1);
			}
		}

		if (oset) {
			if (set == 0) {
				uint32_t set;
				__dup_mem(&set, oset, sizeof(set));
				state_vector.sigmask.sig[0] = set;
			}
			write_mem(oset, sizeof(uint32_t));
		}
	}
	return 0;
}

int SCOPE
replay_sigprocmask(const struct syscall_regs * regs)
{
	int32_t eax = read_int32();
	if (eax >= 0) {
		uintptr_t oset = read_uint32();
		ASSERT(regs->edx == oset, regs, "");
		if (oset != 0) {
			read_mem(oset, sizeof(uint32_t));
		}
#ifdef IN_INJECTOR
		int32_t ret;
		ret = INTERNAL_SYSCALL(sigprocmask, 3,
				regs->ebx, regs->ecx, regs->edx);
		ASSERT(ret == eax, regs, "!@#!@#\n");
#endif
	}
	return eax;
}

#else

void
output_sigprocmask(void)
{
	int32_t ret = read_eax();
	if (ret == 0) {
		int32_t oset;
		read_obj(oset);
		if (oset) {
			skip(sizeof(uint32_t));
		}
	}
	printf("sigprocmask:\t%d\n", read_eax());
}
#endif

