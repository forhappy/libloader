/* 
 * fdbnprintf.h
 * by WN @ Oct. 15, 2010
 */

#ifndef INTERP_FDBNPRINTF_H
#define INTERP_FDBNPRINTF_H

#include "defs.h"
#include <stdarg.h>

extern void ATTR(format(printf, 4, 5))
fdbnprintf(int fd, char * buffer, size_t buf_sz,
		const char * fmt, ...);

extern void
vfdbnprintf(int fd, char * buffer, size_t buf_sz,
		const char * fmt, va_list args);

#endif

// vim:ts=4:sw=4

