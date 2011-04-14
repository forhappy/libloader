/* 
 * fdbnprintf.h
 * by WN @ Oct. 15, 2010
 */

#include <linux/kernel.h>
#include "defs.h"
#include "syscall.h"
#include "fdbnprintf.h"
#include "vsprintf.h"
void
vfdbnprintf(int fd, char * buffer, size_t buf_sz,
		const char * fmt, va_list args)
{
	int r = vsnprintf(buffer, buf_sz, fmt, args);
	if (r > 0)
		sys_write(fd, buffer,
				r > buf_sz ? buf_sz : r);
}

void
fdbnprintf(int fd, char * buffer, size_t buf_sz,
		const char * fmt, ...)
{
	int r __attribute__((unused));
	va_list ap;
	va_start(ap, fmt);
	vfdbnprintf(fd, buffer, buf_sz, fmt, ap);
	va_end(ap);
}

// vim:ts=4:sw=4


