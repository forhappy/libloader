/* 
 * utils.c
 * by WN @ Jun 25, 2009
 */

#include <stdint.h>
#include <stdarg.h>
#include "injector.h"
#include "vsprintf.h"
#include "injector_utils.h"

SCOPE char buffer[BUFFER_SIZE];

SCOPE int
printf(const char * fmt, ...)
{

	va_list args;
	int i;

	va_start(args, fmt);
	i = vsnprintf(buffer, BUFFER_SIZE, fmt, args);
	va_end(args);

	INTERNAL_SYSCALL(write, 3, 1, buffer, i);
	return i;
}

// vim:ts=4:sw=4

