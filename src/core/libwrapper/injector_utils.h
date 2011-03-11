/* 
 * injector_utils.h
 * by WN @ Jun. 25, 2009
 */

#ifndef WRAPPER_UTILS_H
#define WRAPPER_UTILS_H
#include "injector.h"
//#include "string_32.h"
//#include "vsprintf.h"
#include <stdarg.h>
#include <stdio.h>
#define BUFFER_SIZE	(16384)

extern SCOPE char buffer[];

extern SCOPE int
printf(const char * fmt, ...);

extern SCOPE int
printf_int80(const char * fmt, ...);

extern SCOPE int
fdprintf(int fd, const char * fmt, ...);

extern SCOPE int
vfdprintf(int fd, const char * fmt, va_list args);

#endif
