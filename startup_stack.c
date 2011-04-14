/* 
 * startup_stack.c
 * by WN @ Oct. 16, 2010
 */

#include "debug.h"
#include <linux/auxvec.h>

#include "startup_stack.h"


struct startup_stack_info startup_stack_info = {
	.p_argc = NULL,
	.p_args = NULL,
	.p_envs = NULL,
	.p_auxvs = NULL,
	.cmdline_argbegin = NULL,
	.cmdline_argend = NULL,
	.auxv_info = {
		.p_user_entry = NULL,
		.pp_user_phdrs = NULL,
		.p_nr_user_phdrs = NULL,
		.p_base = NULL,
		.p_execfn = NULL,
		.p_sysinfo = NULL,
		.pp_sysinfo_ehdr = NULL,
	},
};

static void
init_auxv_info(void)
{
	assert(STACK_INFO(p_auxvs) != NULL);

	long int * p = STACK_INFO(p_auxvs);

	VERBOSE(LOADER, "auxv start from %p\n", p);
	while (*p != AT_NULL) {
		switch (p[0]) {

#define set_auxvinfo(t, m)	\
		case t: {	\
			VERBOSE(LOADER, #t " at %p\n", &p[1]);\
			STACK_AUXV_INFO(m) = (void*)(&p[1]);\
			break;	\
		}

				set_auxvinfo(AT_ENTRY, p_user_entry);
				set_auxvinfo(AT_BASE, p_base);
				set_auxvinfo(AT_PHDR, pp_user_phdrs);
				set_auxvinfo(AT_PHNUM, p_nr_user_phdrs);
				set_auxvinfo(AT_EXECFN, p_execfn);
				set_auxvinfo(AT_SYSINFO, p_sysinfo);
				set_auxvinfo(AT_SYSINFO_EHDR, pp_sysinfo_ehdr);
#undef set_auxvinfo
		case AT_PHENT: {
			assert(p[1] == sizeof(struct elf_phdr));
			break;
		}
		default:
				break;
		}
		p += 2;
	}
}

void
scan_startup_stack(void * stack_top)
{
	TRACE(LOADER, "in scan_startup_stack\n");
	STACK_INFO(p_argc)= stack_top;
	int argc = *STACK_INFO(p_argc);

	TRACE(LOADER, "argc=%d\n", argc);

	STACK_INFO(p_args) = (void*)(&(STACK_INFO(p_argc)[1]));

	const char ** ptr = STACK_INFO(p_args);
	while (*ptr != NULL) {
		TRACE(LOADER, "arg: %s\n", *ptr);
		ptr ++;
	}

	ptr ++;


	const char * firstenv;
	const char * lastenv;
	STACK_INFO(p_envs) = ptr;
	firstenv = *ptr;
	while (*ptr != NULL) {
		TRACE(LOADER, "env: %s\n", *ptr);
		ptr ++;
	}
	lastenv = *(ptr - 1);

	ptr ++;
	STACK_INFO(p_auxvs) = (void*)ptr;
	init_auxv_info();

	/* about env and args */
	STACK_INFO(cmdline_argbegin) = STACK_INFO(p_args)[0];
	const char * lastarg = STACK_INFO(p_args)[argc - 1];
	STACK_INFO(cmdline_argend) = lastarg + strlen(lastarg) + 1;

	STACK_INFO(envbegin) = firstenv;
	STACK_INFO(envend) = lastenv + strlen(lastenv) + 1;

	const char * cptr = STACK_INFO(envend);
	while (*cptr == '\0')
		cptr ++;
	STACK_INFO(bottom_exec_fn) = cptr;
	TRACE(LOADER, "bottom_exec_fn: %s\n", cptr);
}

// vim:ts=4:sw=4

