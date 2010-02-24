/* 
 * debug.c (host)
 * by WN @ Feb. 22, 2010
 */

#define __DEBUG_C

#include <common/debug.h>
#include <common/assert.h>
#include <signal.h>
#include <stdarg.h>
#include <stdio.h>
#include <unistd.h>

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

	assert(level < NR_DEBUG_LEVELS);
	assert(comp < NR_DEBUG_COMPONENTS);

	if (level < __debug_component_levels[comp])
		return;

#ifdef SNITCHASER_DEBUG
	fprintf(stderr, "[%s %s@%s:%d]\t",
			(char*)__debug_component_names[comp],
			(char*)__debug_level_names[level],
			func, line);
#else
	fprintf(stderr, "%s:\t", __debug_level_names[level]);
#endif

	va_list ap;
	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	va_end(ap);
}

void ATTR(noreturn)
dbg_fatal(void)
{
	raise(SIGABRT);
	exit(-1);
}

#undef __DEBUG_C

// vim:ts=4:sw=4

