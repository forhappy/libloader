
#include "syscall_tbl.h"
#include "log_and_ckpt.h"

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
