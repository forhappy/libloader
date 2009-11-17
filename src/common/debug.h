/* 
 * debug.h
 * by WN @ Nov. 08, 2009
 */

#ifndef __DEBUG_H
#define __DEBUG_H
#include <common/defs.h>
#include <stdlib.h>

__BEGIN_DECLS

enum __debug_level {
	DBG_LV_SILENT	= 0,
	DBG_LV_TRACE,
	DBG_LV_DEBUG,
	DBG_LV_VERBOSE,
	DBG_LV_WARNING,
	DBG_LV_ERROR,
	DBG_LV_FATAL,
	DBG_LV_FORCE,
	NR_DEBUG_LEVELS,
};

#define __DEBUG_H_INCLUDE_DEBUG_COMPONENTS_H
#include <common/debug_components.h>
#undef __DEBUG_H_INCLUDE_DEBUG_COMPONENTS_H


#define __NOP	do {}while(0)

extern void
dbg_init(const char * file);

extern void
dbg_exit(void);

/*
 * we should introduce new directive.
 * if '@' is the 1st char of fmt, the following char is a controller:
 * `q' for suppress prefix;
 *
 * after the controller, user can insert a ' ' to split directives and real
 * format.
 */
extern void
dbg_output(enum __debug_level level,
#ifdef SNITCHASER_DEBUG
		enum __debug_component comp,
		const char * file,
		const char * func,
		int line,
#endif
		char * fmt, ...)
#ifdef SNITCHASER_DEBUG
ATTR(format(printf, 6, 7));
#else
ATTR(format(printf, 2, 3));
#endif
/* Raise a SIGABRT, make program exit */
extern void NORETURN ATTR_NORETURN
dbg_fatal(void);

#ifdef SNITCHASER_DEBUG

#define SILENT(...)     __NOP
#define TRACE(c, ...)	dbg_output(DBG_LV_TRACE, c, __FILE__, __FUNCTION__, __LINE__, __VA_ARGS__)
#define DEBUG(c, ...)	dbg_output(DBG_LV_DEBUG, c, __FILE__, __FUNCTION__, __LINE__, __VA_ARGS__)
#define VERBOSE(c, ...)	dbg_output(DBG_LV_VERBOSE, c, __FILE__, __FUNCTION__, __LINE__, __VA_ARGS__)
#define WARNING(c, ...)	dbg_output(DBG_LV_WARNING, c, __FILE__, __FUNCTION__, __LINE__, __VA_ARGS__)
#define ERROR(c, ...)	dbg_output(DBG_LV_ERROR, c, __FILE__, __FUNCTION__, __LINE__, __VA_ARGS__)
#define FATAL(c, ...)	do {	\
	dbg_output(DBG_LV_FATAL, c, __FILE__, __FUNCTION__, __LINE__, __VA_ARGS__); \
	dbg_fatal();	\
}while(0)
#define FATAL_MSG(c, ...)	dbg_output(DBG_LV_FATAL, c, __FILE__, __FUNCTION__, \
		__LINE__, __VA_ARGS__)
#define FORCE(c, ...)	dbg_output(DBG_LV_FORCE,	\
		c, __FILE__, __FUNCTION__, __LINE__, __VA_ARGS__)

/* memory leak detection */
#define __dbg_info_param	const char * file, const char * func, int line
extern void *
__wrap_malloc(size_t size, __dbg_info_param);
extern void
__wrap_free(void * ptr, __dbg_info_param);
extern void *
__wrap_calloc(size_t count, size_t eltsize, __dbg_info_param);
extern char *
__wrap_strdup(const char * S, __dbg_info_param);
extern void *
__wrap_realloc(void * ptr, size_t newsize, __dbg_info_param);

#ifndef __DEBUG_C
# define malloc(s)	__wrap_malloc(s, __FILE__, __FUNCTION__, __LINE__)
# define calloc(c, s)	__wrap_calloc(c, s, __FILE__, __FUNCTION__, __LINE__)
# define free(ptr)		__wrap_free(ptr, __FILE__, __FUNCTION__, __LINE__)
# define strdup(s)	__wrap_strdup(s, __FILE__, __FUNCTION__, __LINE__)
# define realloc(p, n)	__wrap_realloc(p, n, __FILE__, __FUNCTION__, __LINE__)
#endif

#else

#define SILENT(...)	__NOP
#define TRACE(c, ...)	__NOP
#define DEBUG(c, ...)	__NOP
#define VERBOSE(c, ...)	dbg_output(DBG_LV_VERBOSE, __VA_ARGS__)
#define WARNING(c, ...)	dbg_output(DBG_LV_WARNING, __VA_ARGS__)
#define ERROR(c, ...)	dbg_output(DBG_LV_ERROR, __VA_ARGS__)
#define FATAL(c, ...)	do {	\
	dbg_output(DBG_LV_FATAL, __VA_ARGS__); \
	dbg_fatal();	\
}while(0)
#define FATAL_MSG(c, ...)	dbg_output(DBG_LV_FATAL, __VA_ARGS__)
#define FORCE(c, ...)	dbg_output(DBG_LV_FORCE, __VA_ARGS__)

#endif


__END_DECLS
#endif

