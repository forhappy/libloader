/* 
 * utils.c
 * by WN @ Jun 25, 2009
 */

#include <stdint.h>
#include <stdarg.h>
#include "injector.h"
#include "injector_debug.h"
#include "injector_utils.h"
#include "vsprintf.h"
#include "string_32.h"
 
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

SCOPE int
printf_int80(const char * fmt, ...)
{

	va_list args;
	int i;

	va_start(args, fmt);
	i = vsnprintf(buffer, BUFFER_SIZE, fmt, args);
	va_end(args);

	INTERNAL_SYSCALL_int80(write, 3, 1, buffer, i);
	return i;
}



SCOPE int
fdprintf(int fd, const char * fmt, ...)
{

	va_list args;
	int i;

	va_start(args, fmt);
	i = vsnprintf(buffer, BUFFER_SIZE, fmt, args);
	va_end(args);

	INTERNAL_SYSCALL(write, 3, fd, buffer, i);
	return i;
}

SCOPE int
vfdprintf(int fd, const char * fmt, va_list args)
{
	int i;
	i = vsnprintf(buffer, BUFFER_SIZE, fmt, args);

	INTERNAL_SYSCALL(write, 3, fd, buffer, i);
	return i;
}

// vim:ts=4:sw=4

