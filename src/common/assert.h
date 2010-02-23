/* 
 * common/assert.h
 * by WN @ Feb. 23, 2010
 */

#ifndef __ASSERT_H
#define __ASSERT_H
#include <common/defs.h>

extern void __assert_fail (const char *__assertion, const char *__file,
			   unsigned int __line, const char *__function) ATTR(noreturn);

#ifdef assert
# undef assert
#endif

#define assert(expr)							\
  ((expr)								\
   ? (void)(0)						\
   : __assert_fail (__STRING(expr), __FILE__, __LINE__, __func__))


#endif

// vim:ts=4:sw=4

