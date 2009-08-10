
#include "syscalls.h"


typedef long		__kernel_suseconds_t;
typedef long		__kernel_time_t;
typedef __kernel_suseconds_t	suseconds_t;
typedef __kernel_time_t		time_t;

struct k_timeval {
	time_t		tv_sec;		/* seconds */
	suseconds_t	tv_usec;	/* microseconds */
};

struct	rusage {
	struct k_timeval ru_utime;	/* user time used */
	struct k_timeval ru_stime;	/* system time used */
	long	ru_maxrss;		/* maximum resident set size */
	long	ru_ixrss;		/* integral shared memory size */
	long	ru_idrss;		/* integral unshared data size */
	long	ru_isrss;		/* integral unshared stack size */
	long	ru_minflt;		/* page reclaims */
	long	ru_majflt;		/* page faults */
	long	ru_nswap;		/* swaps */
	long	ru_inblock;		/* block input operations */
	long	ru_oublock;		/* block output operations */
	long	ru_msgsnd;		/* messages sent */
	long	ru_msgrcv;		/* messages received */
	long	ru_nsignals;		/* signals received */
	long	ru_nvcsw;		/* voluntary context switches */
	long	ru_nivcsw;		/* involuntary " */
};


#ifndef SYSCALL_PRINTER

int SCOPE
post_getrusage(const struct syscall_regs * regs)
{
	write_eax(regs);
	if (regs->eax >= 0) {
		write_mem(regs->ecx, sizeof(struct rusage));
	}
	return 0;
}

int SCOPE
replay_getrusage(const struct syscall_regs * regs)
{
	int32_t eax = read_int32();	
	if (eax >= 0) {
		read_mem(regs->ecx, sizeof(struct rusage));
	}
	return eax;
}


#else

void
output_getrusage(void)
{
	int32_t ret = read_eax();
	if (ret >= 0) {
		skip(sizeof(struct rusage));
	}
	printf("getrusage:\t%d\n", ret);
}
#endif

