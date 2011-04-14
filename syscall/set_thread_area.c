
#include "syscall_tbl.h"
#include "log_and_ckpt.h"


int SCOPE
post_set_thread_area(const struct syscall_regs * regs)
{
	
	write_eax(regs);
	return 0;
}

