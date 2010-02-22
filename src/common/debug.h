/* 
 * debug.h
 * by WN @ Nov. 08, 2009
 */

#ifndef __DEBUG_H
#define __DEBUG_H
#include <common/defs.h>
#include <stdlib.h>
#include <stdio.h>
#include <memory.h>	/* strdup */
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

#ifdef __DEBUG_C
static enum __debug_level current_debug_level = DBG_LV_TRACE;
static const char * __debug_level_names[NR_DEBUG_LEVELS] = {
	[DBG_LV_SILENT]		= "SIL",
	[DBG_LV_TRACE]		= "TRC",
	[DBG_LV_DEBUG]		= "DBG",
	[DBG_LV_VERBOSE]	= "VRB",
	[DBG_LV_WARNING]	= "WAR",
	[DBG_LV_ERROR]		= "ERR",
	[DBG_LV_FATAL]		= "FAL",
	[DBG_LV_FORCE]		= "FOC",
};
#endif

#define __NOP	do {}while(0)

extern void
dbg_output(enum __debug_level level,
#ifdef SNITCHASER_DEBUG
		const char * file,
		const char * func,
		int line,
#endif
		char * fmt, ...)
#ifdef SNITCHASER_DEBUG
ATTR(format(printf, 5, 6));
#else
ATTR(format(printf, 2, 3));
#endif

#ifdef SNITCHASER_DEBUG

#define SILENT(...)     __NOP
#define TRACE(...)	dbg_output(DBG_LV_TRACE, __FILE__, __FUNCTION__, __LINE__, __VA_ARGS__)
#define DEBUG(...)	dbg_output(DBG_LV_DEBUG, __FILE__, __FUNCTION__, __LINE__, __VA_ARGS__)
#define VERBOSE(...)	dbg_output(DBG_LV_VERBOSE, __FILE__, __FUNCTION__, __LINE__, __VA_ARGS__)
#define WARNING(...)	dbg_output(DBG_LV_WARNING, __FILE__, __FUNCTION__, __LINE__, __VA_ARGS__)
#define ERROR(...)	dbg_output(DBG_LV_ERROR, __FILE__, __FUNCTION__, __LINE__, __VA_ARGS__)
#define FATAL(...)	do {	\
	dbg_output(DBG_LV_FATAL, __FILE__, __FUNCTION__, __LINE__, __VA_ARGS__); \
	dbg_fatal();	\
}while(0)
#define FATAL_MSG(...)	dbg_output(DBG_LV_FATAL, __FILE__, __FUNCTION__, \
		__LINE__, __VA_ARGS__)
#define FORCE(...)	dbg_output(DBG_LV_FORCE,	\
		__FILE__, __FUNCTION__, __LINE__, __VA_ARGS__)

#else

#define SILENT(...)	__NOP
#define TRACE(...)	__NOP
#define DEBUG(...)	__NOP
#define VERBOSE(...)	dbg_output(DBG_LV_VERBOSE, __VA_ARGS__)
#define WARNING(...)	dbg_output(DBG_LV_WARNING, __VA_ARGS__)
#define ERROR(...)	dbg_output(DBG_LV_ERROR, __VA_ARGS__)
#define FATAL(...)	do {	\
	dbg_output(DBG_LV_FATAL, __VA_ARGS__); \
	dbg_fatal();	\
}while(0)
#define FATAL_MSG(...)	dbg_output(DBG_LV_FATAL, __VA_ARGS__)
#define FORCE(...)	dbg_output(DBG_LV_FORCE, __VA_ARGS__)

#endif


__END_DECLS
#endif

// vim:ts=4:sw=4

