
#include "syscalls.h"


typedef long		__kernel_suseconds_t;
typedef long		__kernel_time_t;
typedef __kernel_suseconds_t	suseconds_t;
typedef __kernel_time_t		time_t;

struct k_timeval {
	time_t		tv_sec;		/* seconds */
	suseconds_t	tv_usec;	/* microseconds */
};

struct k_timezone {
	int	tz_minuteswest;	/* minutes west of Greenwich */
	int	tz_dsttime;	/* type of dst correction */
};

#ifndef SYSCALL_PRINTER

int SCOPE
post_gettimeofday(const struct syscall_regs * regs)
{
	write_eax(regs);
	if (regs->eax >= 0) {
		uintptr_t TP, TZP;
		TP = regs->ebx;
		TZP = regs->ecx;
		write_obj(TP);
		write_obj(TZP);
		
		if (TP != 0)
			write_mem(TP, sizeof(struct k_timeval));
		if (TZP != 0)
			write_mem(TZP, sizeof(struct k_timezone));

	}
	return 0;
}

int SCOPE
replay_gettimeofday(const struct syscall_regs * regs)
{
	int32_t eax = read_int32();	
	if (eax >= 0) {
		uintptr_t TP, TZP;
		read_obj(TP);
		read_obj(TZP);

		ASSERT(TP == regs->ebx, "TP inconsistent\n");
		ASSERT(TZP == regs->ecx, "TZP inconsistent\n");

		if (TP != 0)
			read_mem(TP, sizeof(struct k_timeval));
		if (TZP != 0)
			read_mem(TZP, sizeof(struct k_timezone));
	}
	return eax;
}


#else

void
output_gettimeofday(void)
{
	int32_t ret = read_eax();
	if (ret >= 0) {
		uintptr_t TP, TZP;
		read_obj(TP);
		read_obj(TZP);

		if (TP != 0)
			skip(sizeof(struct k_timeval));
		if (TZP != 0)
			skip(sizeof(struct k_timezone));
	}
	printf("gettimeofday:\t%d\n", ret);
}
#endif

