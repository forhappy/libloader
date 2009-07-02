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

SCOPE char *
readline(int fd)
{
#define READLINE_BUFFER_SIZE	(2048)
#define HALF_READLINE_BUFFER_SIZE	(READLINE_BUFFER_SIZE/2)
	static char readline_buffer[READLINE_BUFFER_SIZE];
#define half_readline_buffer (&readline_buffer[HALF_READLINE_BUFFER_SIZE])
	static char * p_nxtl = readline_buffer;
	static char * p_nxtb = readline_buffer;

	if (p_nxtb == readline_buffer) {
		int sz;
		memset(readline_buffer, '\0', READLINE_BUFFER_SIZE);
		sz = INTERNAL_SYSCALL(read, 3,
				fd, readline_buffer, READLINE_BUFFER_SIZE);
		p_nxtb = readline_buffer + sz;
	}

	char * p;
restart:
	p = p_nxtl;
	while (p < p_nxtb) {
		if ((*p == '\n') || (*p == '\0')) {
			char * p_nxtl_save = p_nxtl;
			if (*p == '\n')
				p_nxtl = p+1;
			else
				p_nxtl = p;
			*p = '\0';
			return p_nxtl_save;
		}
		p++;
	}

	/* file is short enough that only half of
	 * the readline buffer is OK. */
	if (p < readline_buffer + READLINE_BUFFER_SIZE)
		return p;

	int sz;
	if (p_nxtl < half_readline_buffer) {
		INJ_WARNING("line too long: %s\n", p_nxtl);
		p_nxtl = half_readline_buffer;
	}


	/* move the bottom half to the upper half */
	memcpy(readline_buffer, half_readline_buffer,
			HALF_READLINE_BUFFER_SIZE);
	memset(half_readline_buffer, '\0', HALF_READLINE_BUFFER_SIZE);
	p_nxtl -= HALF_READLINE_BUFFER_SIZE;
	sz = INTERNAL_SYSCALL(read, 3,
			fd, half_readline_buffer, HALF_READLINE_BUFFER_SIZE);
	p_nxtb = half_readline_buffer + sz;
	goto restart;
}

// vim:ts=4:sw=4

