/*
 * =====================================================================================
 *
 *       Filename:  syscall_tbl.c
 *
 *    Description: syscall table 
 *
 *        Version:  1.0
 *        Created:  04/12/2011 04:28:16 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  HP.Fu 
 *        Company:  ICT
 *
 * =====================================================================================
 */
#include "syscall_tbl.h"
#include "syscall_nr.h"
struct syscall_tabent syscall_table[NR_SYSCALLS + 1] = {
	def_syscall_entry(brk, NULL),
	def_syscall_entry(mmap, NULL),
	def_syscall_entry(mmap2, NULL),
	def_syscall_entry(fstat64, NULL),
	def_syscall_entry(close, NULL),
	def_syscall_entry(stat64, NULL),
	def_syscall_entry(read, NULL),
	def_syscall_entry(open, NULL),
	def_syscall_entry(mprotect, NULL),
	def_syscall_entry(munmap, NULL),
	def_syscall_entry(write, NULL),
	def_syscall_entry(ioctl, NULL),
	def_syscall_entry(gettimeofday, NULL),
	def_syscall_entry(getpid, NULL),
	def_syscall_entry(rt_sigprocmask, NULL),
	def_syscall_entry(rt_sigaction, NULL),
	def_syscall_entry(nanosleep, NULL),
	def_syscall_entry(set_tid_address, NULL),
	def_syscall_entry(set_robust_list, NULL),
	def_syscall_entry(set_thread_area, NULL),
	def_syscall_entry(futex, NULL),
	def_syscall_entry(ugetrlimit, NULL),
	def_syscall_entry(uname, NULL),
	[NR_SYSCALLS]	= {NULL, NULL, NULL},
};

