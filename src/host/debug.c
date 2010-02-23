/* 
 * debug.c (host)
 * by WN @ Feb. 22, 2010
 */

#define __DEBUG_C

#include <common/debug.h>
#include <stdarg.h>
#include <stdio.h>
#include <unistd.h>

void
dbg_output(enum __debug_level level,
#ifdef SNITCHASER_DEBUG
		const char * file ATTR(unused),
		const char * func,
		int line,
#endif
		char * fmt, ...)
{
	if (fmt == NULL)
		return;
	if (level < current_debug_level)
		return;

#ifdef SNITCHASER_DEBUG
	fprintf(stderr, "[%s@%s:%d]\t",
			__debug_level_names[level],
			func, line);
#else
	fprintf(stderr, "%s:\t", __debug_level_names[level]);
#endif

	va_list ap;
	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	va_end(ap);
}

#undef __DEBUG_C

// vim:ts=4:sw=4

