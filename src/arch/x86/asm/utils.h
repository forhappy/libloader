/* 
 * asm/utils.h
 */

#ifndef __ASM_UTILS_H
#define __ASM_UTILS_H

#include <common/defs.h>
#include <asm/syscall.h>
#include <stdarg.h>

extern int ATTR(format(printf, 2, 3))
fdprintf(int fd, const char * fmt, ...);

extern int
vfdprintf(int fd, const char * fmt, va_list args);

extern int ATTR(format(printf, 1, 2))
__printf(const char * fmt, ...);

extern void
relocate_interp(void);

static ATTR(noreturn) inline void
__exit(int n)
{
	INTERNAL_SYSCALL_int80(exit, 1, n);
	while(1);
}

#endif
// vim:ts=4:sw=4

