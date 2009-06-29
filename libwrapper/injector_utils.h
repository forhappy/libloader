/* 
 * injector_utils.h
 * by WN @ Jun. 25, 2009
 */

#ifndef WRAPPER_UTILS_H
#define WRAPPER_UTILS_H
#include "injector.h"
#include "vsprintf.h"

#define BUFFER_SIZE	(16384)

extern SCOPE char buffer[];

extern SCOPE int
printf(const char * fmt, ...);

#endif

