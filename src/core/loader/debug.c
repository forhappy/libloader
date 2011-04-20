/* 
 * loader/debug.c
 * by WN @ Oct. 15, 2010
 * modified by HP.Fu @ Mar. 28, 2011
 */


#define __DEBUG_C
#include <defs.h>
#include <debug.h>
#include <syscall.h>
#include <loader/fdbnprintf.h>

#define DEBUG_BUFFER_SZ	(256)

#ifndef STDERR_FILENO
# define STDERR_FILENO (2)
#endif

void ATTR(noreturn)
__assert_fail (const char *__assertion, const char *__file,
		unsigned int __line, const char *__function)
{
	char buffer[DEBUG_BUFFER_SZ];
	fdbnprintf(DBG_OUTPUT_FILENO, buffer, DEBUG_BUFFER_SZ,
			"** %s:%d: %s: assertion `%s' failed **\n",
			__file, __line, __function, __assertion);
	breakpoint();
	sys_exit(-1);
}


void
dbg_init(void)
{
	sys_close(DBG_OUTPUT_FILENO);
	sys_dup2(STDERR_FILENO, DBG_OUTPUT_FILENO);
}


void
dbg_output(enum __debug_level level,
#ifdef SNITCHASER_DEBUG
		enum __debug_component comp,
		const char * file ATTR(unused),
		const char * func,
		int line,
#endif
		char * fmt, ...)
{
	if (fmt == NULL)
		return;
	if (level == DBG_LV_SILENT)
		return;

	assert(level < NR_DEBUG_LEVELS);
#ifdef SNITCHASER_DEBUG
	assert(comp < NR_DEBUG_COMPONENTS);
	if (level < __debug_component_levels[comp])
		return;
#endif

	char buffer[DEBUG_BUFFER_SZ];

#ifdef SNITCHASER_DEBUG
	fdbnprintf(DBG_OUTPUT_FILENO, buffer, DEBUG_BUFFER_SZ,
			"[%s %s@%s:%d]:\t",
			(char*)__debug_component_names[comp],
			(char*)__debug_level_names[level],
			func, line);
#else
	fdbnprintf(DBG_OUTPUT_FILENO, buffer, DEBUG_BUFFER_SZ,
			"%s:\t", (char*)__debug_level_names[level]);
#endif

	va_list ap;
	va_start(ap, fmt);
	vfdbnprintf(DBG_OUTPUT_FILENO, buffer, DEBUG_BUFFER_SZ,
			fmt, ap);
	va_end(ap);
}


void ATTR(noreturn)
dbg_fatal(void)
{
	breakpoint();
	sys_exit(-1);
}

// vim:ts=4:sw=4

