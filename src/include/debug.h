#ifndef _LOADER_DEBUG_H
#define _LOADER_DEBUG_H
#include <defs.h>
#include <xdebug.h>
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

#define __DEBUG_INCLUDE_DEBUG_COMPONENTS_H
#define def_dbg_component(__comp, __name, __level)	__comp,
enum __debug_component {
#include <debug_components.h>
	NR_DEBUG_COMPONENTS,
};
#undef def_dbg_component
#undef __DEBUG_INCLUDE_DEBUG_COMPONENTS_H

#define __NOP	do {} while(0)

extern void
dbg_init(void);

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
ATTR(format(printf, 6, 7)) ATTR_HIDDEN;
#else
ATTR(format(printf, 2, 3)) ATTR_HIDDEN;
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
#define FATAL_MSG(c, ...)	dbg_output(DBG_LV_FATAL, __VA_ARGS__)
#define FORCE(c, ...)	dbg_output(DBG_LV_FORCE, __VA_ARGS__)

#endif

#define CASSERT(___cond, ___comp, fmt...)		do {	\
	if (!(___cond)) 				\
		FATAL(___comp, fmt);		\
} while(0)

#define EASSERT(___comp, fmt...)	do {		\
	if (errno != 0)				\
		FATAL(___comp, fmt);		\
} while(0)



__END_DECLS


#ifdef __DEBUG_C
#undef __DEBUG_C
static const char * __debug_level_names[NR_DEBUG_LEVELS] = {
	[DBG_LV_SILENT]		= "SILENT",
	[DBG_LV_TRACE]		= "TRACE",
	[DBG_LV_DEBUG]		= "DEBUG",
	[DBG_LV_VERBOSE]	= "VERBOSE",
	[DBG_LV_WARNING]	= "WARNING",
	[DBG_LV_ERROR]		= "ERROR",
	[DBG_LV_FATAL]		= "FATAL",
	[DBG_LV_FORCE]		= "FORCE",
};

#ifdef SNITCHASER_DEBUG
#define __DEBUG_INCLUDE_DEBUG_COMPONENTS_H
#define def_dbg_component(__comp, __name, __level)	[__comp] = __name,
static const char *
__debug_component_names[NR_DEBUG_COMPONENTS] = {
#include <debug_components.h>
};
#undef def_dbg_component
#define def_dbg_component(__comp, __name, __level)	\
	[__comp] = DBG_LV_##__level,
static const enum __debug_level
__debug_component_levels[NR_DEBUG_COMPONENTS] = {
#include <debug_components.h>
};
#undef def_dbg_component
#undef __DEBUG_INCLUDE_DEBUG_COMPONENTS_H

#define get_level_name(l)	(__debug_level_names[l])
#define get_comp_name(c)	(__debug_component_names[c])
#define get_comp_level(c)	(__debug_component_levels[c])

#endif

#endif

#endif
