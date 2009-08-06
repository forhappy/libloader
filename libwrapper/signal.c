/* 
 * signal.c
 * by WN @ Aug. 06, 2009
 */

#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>

#include "injector.h"
#include "checkpoint/checkpoint.h"
#include "injector_utils.h"
#include "injector_debug.h"
#include "pthread_defs.h"

extern void __vsyscall();

void SCOPE
do_wrapped_rt_sigreturn(void)
{
	INJ_WARNING("rt_sigreturn\n");
	return;
}


void SCOPE
do_wrapped_sigreturn(void)
{
	INJ_WARNING("sigreturn\n");
	INTERNAL_SYSCALL(exit, 1, 10);
	return;
}
