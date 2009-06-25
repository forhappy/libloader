/* 
 * vsprintf.h
 * by WN @ Jun. 25, 2009
 */

#ifndef WRAPPER_PRINTF_H
#define WRAPPER_PRINTF_H

#include <stdarg.h>
#include "injector.h"

extern SCOPE long
simple_strtol(const char *cp,char **endp,unsigned int base);

extern SCOPE long long
simple_strtoll(const char *cp,char **endp,unsigned int base);

extern SCOPE unsigned long
simple_strtoul(const char *cp,char **endp,unsigned int base);

extern SCOPE unsigned long long
simple_strtoull(const char *cp,char **endp,unsigned int base);

extern SCOPE int
vsnprintf(char *buf, size_t size, const char *fmt, va_list args);

extern SCOPE int
snprintf(char * buf, size_t size, const char *fmt, ...);

extern SCOPE int
sprintf(char * buf, const char *fmt, ...);

extern SCOPE int
sscanf(const char * buf, const char * fmt, ...);

extern SCOPE int
vsscanf(const char * buf, const char * fmt, va_list args);

extern SCOPE char
*strcat(char * dest,const char * src);

extern SCOPE char
*strchr(const char * s, int c);

extern SCOPE int
strcmp(const char * cs,const char * ct);

extern SCOPE char*
strcpy(char * dest,const char *src);

extern SCOPE int
strict_strtol(const char *cp, unsigned int base, long *res);

extern SCOPE int
strict_strtoll(const char *cp, unsigned int base, long long *res);

extern SCOPE int
strict_strtoul(const char *cp, unsigned int base, unsigned long *res);

extern SCOPE int
strict_strtoull(const char *cp, unsigned int base, unsigned long long *res);

extern SCOPE size_t
strlen(const char * s);

extern SCOPE char *
strncat(char * dest,const char * src,size_t count);

extern SCOPE int
strncmp(const char * cs,const char * ct,size_t count);

extern SCOPE char *
strncpy(char * dest,const char *src,size_t count);

extern SCOPE size_t
strnlen(const char *s, size_t count);

extern SCOPE int
vscnprintf(char *buf, size_t size, const char *fmt, va_list args);

extern SCOPE int
vsprintf(char *buf, const char *fmt, va_list args);


#endif
