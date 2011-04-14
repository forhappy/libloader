/*
 * =====================================================================================
 *
 *       Filename:  wrapper.c
 *
 *    Description: syscall wrapper 
 *
 *        Version:  1.0
 *        Created:  04/10/2011 07:44:46 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  HP.Fu 
 *        Company:  ICT
 *
 * =====================================================================================
 */
#include "defs.h"
#include "interp_main.h"
#include "debug.h"
#include "syscall_tbl.h"
#include "log_and_ckpt.h"
extern int logger_fd;
void syscall_pre_handler(const struct syscall_regs reg);
void syscall_post_handler(const struct syscall_regs reg);

extern struct syscall_tabent syscall_table[];

/* before we do real vsyscall, we must restore register
 * state */

#define do_real_syscall(__reg, __retval) \
		asm volatile(	\
				"pushl %%eax\n"	\
				"pushl %%ebx\n"	\
				"pushl %%ecx\n"	\
				"pushl %%edx\n"	\
				"pushl %%esi\n"	\
				"pushl %%edi\n"	\
				"pushl %%ebp\n"	\
				"movl %P1, %%eax\n"	\
				"movl %P2, %%ebx\n"	\
				"movl %P3, %%ecx\n"	\
				"movl %P4, %%edx\n" \
				"movl %P5, %%esi\n"	\
				"movl %P6, %%edi\n"	\
				"movl %P7, %%ebp\n"	\
				"int $0x80\n" \
				"popl %%ebp\n"	\
				"popl %%edi\n"	\
				"popl %%esi\n"	\
				"popl %%edx\n" \
				"popl %%ecx\n"	\
				"popl %%ebx\n"	\
				"addl $0x4, %%esp\n"	/* eax */	\
				"movl %%eax, %P1\n"	\
				: "=a" ((__retval))	\
				: "m" ((__reg).eax), "m" ((__reg).ebx), "m" ((__reg).ecx), 	\
				"m" ((__reg).edx), "m" ((__reg).esi), "m" ((__reg).edi),	\
				"m" ((__reg).ebp));

void wrapped_syscall_entry(void)
{
#if 1 
asm ("\
		pushl %esp\n\
		pushl 8(%esp)\n\
		pushl %eax\n\
		pushf\n\
		pushl %ebp\n\
		pushl %edi\n\
		pushl %esi\n\
		pushl %edx\n\
		pushl %ecx\n\
		pushl %ebx\n\
		pushl %eax\n\
		call wrapped_syscall\n\
		popl %eax\n\
		popl %ebx\n\
		popl %ecx\n\
		popl %edx\n\
		popl %esi\n\
		popl %edi\n\
		popl %ebp\n\
		popf\n\
		addl $0xC, %esp\n\
		");
#endif
return;
}	

void wrapped_syscall(const struct syscall_regs reg)
{
	/* pre syscall */
	int retval;
	syscall_pre_handler(reg);
	/* do real syscall */
	do_real_syscall(reg, retval);
	syscall_post_handler(reg);
return;
}

void syscall_pre_handler(const struct syscall_regs reg)
{
	//VERBOSE(LOADER, "syscall number: %d\n", reg.eax);
	write_syscall_nr(reg.orig_eax);
	if (syscall_table[reg.orig_eax].pre_handler != NULL)
		return syscall_table[reg.orig_eax].pre_handler(&reg);
}

void syscall_post_handler(const struct syscall_regs reg)
{
	//VERBOSE(LOADER, "return value: 0x%x\n", reg.eax);

	CASSERT(syscall_table[reg.orig_eax].post_handler != NULL, LOG_SYSCALL, 
			"no such syscall post-handler: %d\n", reg.orig_eax);

	return syscall_table[reg.orig_eax].post_handler(&reg);
}

