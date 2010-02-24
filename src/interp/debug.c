/* 
 * debug.c (interp)
 * by WN @ Feb. 22, 2010
 */
#define __DEBUG_C

#include <common/debug.h>
#include <common/assert.h>
#include <asm/utils.h>
#include <stdarg.h>
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
	fdprintf(STDERR_FILENO, "[%s@%s:%d]:\t",
			(char*)__debug_level_names[level],
			func, line);
#else
	fdprintf(STDERR_FILENO, "%s:\t", (char*)__debug_level_names[level]);
#endif

	va_list ap;
	va_start(ap, fmt);
	vfdprintf(STDERR_FILENO, fmt, ap);
	va_end(ap);
}

void ATTR(noreturn)
__assert_fail (const char *__assertion, const char *__file,
		unsigned int __line, const char *__function)
{
	fdprintf(STDERR_FILENO, 
			"** %s:%d: %s: assertion `%s' failed **\n",
			__file, __line, __function, __assertion);
	__exit(-1);
}

void ATTR(noreturn)
dbg_fatal(void)
{
	__exit(-1);
}

#undef __DEBUG_C

// vim:ts=4:sw=4

