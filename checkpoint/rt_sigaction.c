
#include "syscalls.h"
#ifndef SYSCALL_PRINTER
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

int SCOPE
replay_rt_sigaction(const struct syscall_regs * regs)
{
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

void
output_rt_sigaction(void)
{
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

