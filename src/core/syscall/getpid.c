/*
 * =====================================================================================
 *
 *       Filename:  getpid.c
 *
 *    Description:  getpid syscall 
 *
 *        Version:  1.0
 *        Created:  04/13/2011 11:25:40 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  HP.Fu
 *        Company:  ICT
 *
 * =====================================================================================
 */
#include "syscall_tbl.h"
#include "log_and_ckpt.h"
int SCOPE
post_getpid(const struct syscall_regs * regs)
{
	write_eax(regs);
	return 0;
}


