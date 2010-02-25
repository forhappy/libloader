/* 
 * debug.h
 * by WN @ Nov. 08, 2009
 */

#ifndef __DEBUG_H
#define __DEBUG_H
#include <common/defs.h>
#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>

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


enum __debug_component {
	SYSTEM	= 0,
	LOADER,
	NR_DEBUG_COMPONENTS,
};

#ifdef __DEBUG_C
static const char * __debug_level_names[NR_DEBUG_LEVELS] ATTR(unused) = {
	[DBG_LV_SILENT]		= "SIL",
	[DBG_LV_TRACE]		= "TRC",
	[DBG_LV_DEBUG]		= "DBG",
	[DBG_LV_VERBOSE]	= "VRB",
	[DBG_LV_WARNING]	= "WAR",
	[DBG_LV_ERROR]		= "ERR",
	[DBG_LV_FATAL]		= "FAL",
	[DBG_LV_FORCE]		= "FOC",
};

#ifdef SNITCHASER_DEBUG
static const char *
__debug_component_names[NR_DEBUG_COMPONENTS] = {
	[SYSTEM]	= "SYS",
	[LOADER]	= "LOD",
};
#endif

static const enum __debug_level
__debug_component_levels[NR_DEBUG_COMPONENTS] = {
	[SYSTEM]	= DBG_LV_TRACE,
	[LOADER]	= DBG_LV_TRACE,
};


#endif

#define __NOP	do {} while(0)

extern void
dbg_output(enum __debug_level level,
#ifdef SNITCHASER_DEBUG
		enum  __debug_component comp,
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

extern void
dbg_fatal(void) ATTR(noreturn);

#ifdef SNITCHASER_DEBUG

#define SILENT(...)     __NOP
#define TRACE(c, ...)	dbg_output(DBG_LV_TRACE, (c), __FILE__, __FUNCTION__, __LINE__, __VA_ARGS__)
#define DEBUG(c, ...)	dbg_output(DBG_LV_DEBUG, (c), __FILE__, __FUNCTION__, __LINE__, __VA_ARGS__)
#define VERBOSE(c, ...)	dbg_output(DBG_LV_VERBOSE, (c), __FILE__, __FUNCTION__, __LINE__, __VA_ARGS__)
#define WARNING(c, ...)	dbg_output(DBG_LV_WARNING, (c), __FILE__, __FUNCTION__, __LINE__, __VA_ARGS__)
#define ERROR(c, ...)	dbg_output(DBG_LV_ERROR, (c), __FILE__, __FUNCTION__, __LINE__, __VA_ARGS__)
#define FATAL(c, ...)	do {	\
	dbg_output(DBG_LV_FATAL, (c), __FILE__, __FUNCTION__, __LINE__, __VA_ARGS__); \
	dbg_fatal();	\
} while(0)
#define FATAL_MSG(c, ...)	dbg_output(DBG_LV_FATAL, (c), __FILE__, __FUNCTION__, \
		__LINE__, __VA_ARGS__)
#define FORCE(c, ...)	dbg_output(DBG_LV_FORCE, (c), \
		__FILE__, __FUNCTION__, __LINE__, __VA_ARGS__)

#else

#define SILENT(...)	__NOP
#define TRACE(...)	__NOP
#define DEBUG(...)	__NOP
#define VERBOSE(c, ...)	dbg_output(DBG_LV_VERBOSE, __VA_ARGS__)
#define WARNING(c, ...)	dbg_output(DBG_LV_WARNING, __VA_ARGS__)
#define ERROR(c, ...)	dbg_output(DBG_LV_ERROR, __VA_ARGS__)
#define FATAL(c, ...)	do {	\
	dbg_output(DBG_LV_FATAL, __VA_ARGS__); \
	dbg_fatal();	\
}while(0)
#define FATAL_MSG(...)	dbg_output(DBG_LV_FATAL, __VA_ARGS__)
#define FORCE(...)	dbg_output(DBG_LV_FORCE, __VA_ARGS__)

#endif


__END_DECLS
#endif

// vim:ts=4:sw=4
