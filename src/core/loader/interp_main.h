/*
 * =====================================================================================
 *
 *       Filename:  interp_main.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/10/2011 07:57:52 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *        Company:  
 *
 * =====================================================================================
 */
#ifndef __INTERP_MAIN_H
#define __INTERP_MAIN_H


#include <config.h>
#include <defs.h>
#include <debug.h>
#include <syscall.h>

#include <linux/kernel.h>
#include <linux/personality.h>

#include <loader/processor.h>
/* use volatile to prevent optmization */
struct interp_startup_stack {
	volatile struct pusha_regs saved_regs;
	void * volatile stack_top;
};

extern unsigned int 
get_vsyscall_entry(const char **envp);

extern void 
set_vsyscall_entry(const char **envp, unsigned long new_entry);

extern unsigned long vsyscall_entry;

#endif
