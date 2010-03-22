/* 
 * vsprintf.h
 * by WN @ Jun. 25, 2009
 */

#ifndef WRAPPER_PRINTF_H
#define WRAPPER_PRINTF_H

#include <stdarg.h>
#include <xasm/string.h>

extern long
simple_strtol(const char *cp,char **endp,unsigned int base);

extern long long
simple_strtoll(const char *cp,char **endp,unsigned int base);

extern unsigned long
simple_strtoul(const char *cp,char **endp,unsigned int base);

extern unsigned long long
simple_strtoull(const char *cp,char **endp,unsigned int base);

extern int
vsnprintf(char *buf, size_t size, const char *fmt, va_list args);

extern int
snprintf(char * buf, size_t size, const char *fmt, ...);

extern int
sprintf(char * buf, const char *fmt, ...);

extern int
sscanf(const char * buf, const char * fmt, ...);

extern int
vsscanf(const char * buf, const char * fmt, va_list args);

extern int
strict_strtol(const char *cp, unsigned int base, long *res);

extern int
strict_strtoll(const char *cp, unsigned int base, long long *res);

extern int
strict_strtoul(const char *cp, unsigned int base, unsigned long *res);

extern int
strict_strtoull(const char *cp, unsigned int base, unsigned long long *res);

extern int
vscnprintf(char *buf, size_t size, const char *fmt, va_list args);

extern int
vsprintf(char *buf, const char *fmt, va_list args);

#endif

