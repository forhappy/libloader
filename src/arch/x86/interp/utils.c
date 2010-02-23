/* 
 * asm/debug.c
 */

#include <asm/syscall.h>
#include <asm/vsprintf.h>
#include <asm/utils.h>

static char fdprintf_buf[1024];

int
vfdprintf(int fd, const char * fmt, va_list args)
{
	int n = vsnprintf(fdprintf_buf, 1024, fmt, args);
	INTERNAL_SYSCALL_int80(write, 3, fd, fdprintf_buf, n);
	return n;
}

int
fdprintf(int fd, const char * fmt, ...)
{
	int n;
	va_list ap;
	va_start(ap, fmt);
	n = vfdprintf(fd, fmt, ap);
	va_end(ap);
	return n;
}

int
printf(const char * fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	int n = vfdprintf(2, fmt, ap);
	va_end(ap);
	return n;
	
}

// vim:ts=4:sw=4

