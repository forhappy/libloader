/* 
 * startup_stack.h
 * by WN @ Oct. 16, 2010
 */

#ifndef LOADER_INIT_STACK_H
#define LOADER_INIT_STACK_H

#include <defs.h>
#include <linux/elf.h>
#include <loader/elf.h>
#include <linux/string.h>
extern void
scan_startup_stack(void * startup_stack_top);

struct auxv_info {
	/* entries in aux vector */
	void ** p_user_entry;
	struct elf_phdr ** pp_user_phdrs;
	int * p_nr_user_phdrs;
	void ** p_base;
	const char ** p_execfn;
	void ** p_sysinfo;
	struct elf_phdr ** pp_sysinfo_ehdr;
};

struct startup_stack_info {
	long int * p_argc;
	const char ** p_args;
	const char ** p_envs;
	long int * p_auxvs;
	const char * cmdline_argbegin;
	const char * cmdline_argend;
	const char * envbegin;
	const char * envend;
	const char * bottom_exec_fn;
	struct auxv_info auxv_info;
};

extern struct startup_stack_info
startup_stack_info;

#define STACK_INFO(x)	(startup_stack_info.x)
#define STACK_AUXV_INFO(x)	(STACK_INFO(auxv_info).x)

#endif

// vim:ts=4:sw=4

