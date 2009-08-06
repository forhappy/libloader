
#include "syscalls.h"
#ifndef SYSCALL_PRINTER

/* 
 * in the loader, those 2 vars are defined in currf2.c;
 * in the injector, those 2 vars are defined in wrapper.c;
 */

#ifndef IN_INJECTOR
extern SCOPE uint32_t wrapped_sigreturn;
extern SCOPE uint32_t wrapped_rt_sigreturn;
#else
extern void wrapped_sigreturn(void);
extern void wrapped_rt_sigreturn(void);
#endif

#define SIG_DFL	((void*)(0))	/* default signal handling */
#define SIG_IGN	((void*)(1))	/* ignore signal */
#define SIG_ERR	((void*)(-1))	/* error return from signal */

#define SA_NOCLDSTOP	0x00000001u
#define SA_NOCLDWAIT	0x00000002u
#define SA_SIGINFO	0x00000004u
#define SA_ONSTACK	0x08000000u
#define SA_RESTART	0x10000000u
#define SA_NODEFER	0x40000000u
#define SA_RESETHAND	0x80000000u

#define SA_NOMASK	SA_NODEFER
#define SA_ONESHOT	SA_RESETHAND

#define SA_RESTORER	0x04000000

int SCOPE
pre_rt_sigaction(const struct syscall_regs * regs)
{
	/* if the signal handler is not the SIG_DFL or SIG_IGN, we need to
	 * reset the restore flag, make the target call my wrapper after signal
	 * handler exit. */
	uintptr_t act = regs->ecx;
	struct k_sigaction d;
	/* check act */
	if (act == 0)
		return 0;
	__dup_mem(&d, act, sizeof(d));

	if ((d.sa_handler == SIG_IGN) || (d.sa_handler == SIG_DFL))
		return 0;

	/* check sa_flags, whether the handler use itself restorer */
	ASSERT(!(d.sa_flags & SA_RESTORER) || (d.sa_restorer == (void*)wrapped_rt_sigreturn),
			"handler for signal %d use itself's restorer, doesn't support\n");

	/* now update sa_flags and sa_restorer */
	d.sa_flags |= SA_RESTORER;
	d.sa_restorer = (void*)wrapped_rt_sigreturn;

	/* update mem */
	__upd_mem(act, &d, sizeof(d));

	return 0;
}



int SCOPE
post_rt_sigaction(const struct syscall_regs * regs)
{
	write_eax(regs);
	if (regs->eax == 0) {
		void * d = &state_vector.sigactions[regs->ebx];
		uintptr_t act = regs->ecx;
		uintptr_t oact = regs->edx;
		int sigsetsize = regs->esi;

		write_obj(sigsetsize);
		write_obj(oact);
		write_obj(act);

		if (sigsetsize != sizeof(k_sigset_t)) {
			INJ_WARNING("esi (%d) != %d\n",
					sigsetsize, sizeof(k_sigset_t));
			return 0;
		}

		if (act != 0)
			__dup_mem(d, act, sizeof(struct k_sigaction));

		if (oact != 0) {
			if (act == 0)
				__dup_mem(d, oact, sizeof(struct k_sigaction));
			write_mem(oact, sizeof(struct k_sigaction));
		}
	}
	return 0;
}

static void
replay_trap(const struct syscall_regs * regs)
{
	INJ_FATAL("eip=0x%x\n", regs->eip);
	INJ_FATAL("esp=0x%x\n", regs->esp);
	INJ_FATAL("ebp=0x%x\n", regs->ebp);
	asm volatile (
			"movl %0, %%esp\n"
			"movl %1, %%ebp\n" : : "m" (regs->esp), "m" (regs->ebp));
	asm volatile ("int3\n");
	__exit(-1);
}

int SCOPE
replay_rt_sigaction(const struct syscall_regs * regs)
{
	/* we need to check signal first */
	int16_t flag = read_int16();
	if (flag != -1) {
		/* signal distrub me */
		INJ_WARNING("rt_sigaction distrubed by a signal, this logger has over. switch a ckpt.\n");
		replay_trap(regs);
	}
	int32_t eax = read_int32();
	if (eax == 0) {
		int sigsetsize = read_int32();
		ASSERT(sigsetsize == regs->esi, "");
		ASSERT(sigsetsize == sizeof(k_sigset_t), "!@@#1\n");

		uintptr_t oact = read_uint32();
		ASSERT(oact == regs->edx, "");
		
		uintptr_t act = read_uint32();
		ASSERT(act == regs->ecx, "");

		if (oact != 0)
			read_mem(oact, sizeof(struct k_sigaction));
	}
	return eax;
}

#else

extern int finished;
void
output_rt_sigaction(void)
{
	int16_t sigflag = read_int16();
	if (sigflag != -1) {
		printf("rt_sigaction distrubed by signal\n");
		finished = 1;
		return;
	}
	int32_t ret = read_eax();
	if (ret == 0) {
		int sigsetsize;
		uintptr_t act;
		uintptr_t oact;
		read_obj(sigsetsize);
		read_obj(oact);
		read_obj(act);

		if (sigsetsize == sizeof(k_sigset_t)) {
			if (oact != 0)
				skip(sizeof(struct k_sigaction));
		}
	}
	printf("rt_sigaction:\t%d\n", ret);
}
#endif

