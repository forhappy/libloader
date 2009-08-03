/* Wang Nan @ Jan 24, 2009 */
#ifndef CURRF2_DEBUG_H
#define CURRF2_DEBUG_H

#include <sys/cdefs.h>

#include "defs.h"
#include "injector.h"

__BEGIN_DECLS


/* use defferent stdout and stderr.
 * if the target close those 2 files,
 * injector's err msg can still be display.
 * below 2 fds cannot be 1023 (which is logger_fd),
 * and they are limited by rlimit. at least in
 * my system, by the default configuration,
 * 1024 is too large, 1021 and 1022 are just OK.
 * */
#define STDOUT_FILENO_INJ	(1021)
#define STDERR_FILENO_INJ	(1022)

enum debug_level {
	SILENT = 0,
	TRACE,
	VERBOSE,
	WARNING,
	ERROR,
	FATAL,
	FORCE,
	NR_DEBUG_LEVELS
};

enum debug_component {
	INJECTOR,
	NR_COMPONENTS
};

#ifdef CURRF2_DEBUG_C

static const char * debug_comp_name[NR_COMPONENTS] = {
	[INJECTOR] = "INJ",
};

static enum debug_level debug_levels[NR_COMPONENTS] = {
	[INJECTOR] = VERBOSE,
};
#endif

#ifndef CURRF2_DEBUG_OFF
extern SCOPE void debug_out(int prefix, enum debug_level level, enum debug_component comp,
	       const char * func_name, int line_no,
	       const char * fmt, ...);
extern SCOPE void set_comp_level(enum debug_component comp, enum debug_level level);
extern SCOPE enum debug_level get_comp_level(enum debug_component comp);
#else
# define debug_out(z, a, b, c, d, e,...) do{} while(0)
# define set_comp_level(a, b)   do {} while(0)
# define get_comp_level(a)   ({SILENT;})
#endif

#define DEBUG_INIT(file)	do { debug_init(file);} while(0)
#define DEBUG_MSG(level, comp, str...) do{ debug_out(1, level, comp, __FUNCTION__, __LINE__, str); } while(0)
/*  Don't output prefix */
#define DEBUG_MSG_CONT(level, comp, str...) do{ debug_out(0, level, comp, __FUNCTION__, __LINE__, str); } while(0)
#define DEBUG_SET_COMP_LEVEL(mask, level) do { set_comp_level(mask, level); } while(0)

/* XXX */
/* Below definition won't distrub the debug level, because 
 * they are all func-like macro */
#define TRACE(comp, str...) DEBUG_MSG(TRACE, comp, str)
#ifndef CURRF2_DEBUG_OFF
# define VERBOSE(comp, str...) DEBUG_MSG(VERBOSE, comp, str)
# define WARNING(comp, str...) DEBUG_MSG(WARNING, comp, str)
# define WARNING_CONT(comp, str...) DEBUG_MSG_CONT(WARNING, comp, str)
# define ERROR(comp, str...) DEBUG_MSG(ERROR, comp, str)
# define FATAL(comp, str...) DEBUG_MSG(FATAL, comp, str)
# define FORCE(comp, str...) DEBUG_MSG(FORCE, comp, str)
# define FORCE_CONT(comp, str...) DEBUG_MSG_CONT(FORCE, comp, str)
#else
extern SCOPE void message_out(int prefix, enum debug_level, enum debug_component, char * fmt, ...);
# define VERBOSE(c, s...)	message_out(1, VERBOSE, c, s)
# define WARNING(c, s...)	message_out(1, WARNING, c, s)
# define WARNING_CONT(c, s...)	message_out(0, WARNING, c, s)
# define ERROR(c, s...)		message_out(1, ERROR, c, s)
# define FATAL(c, s...)		message_out(1, FATAL, c, s)
# define FORCE(c, s...)		message_out(1, FORCE, c, s)
# define FORCE_CONT(comp, str...) message_out(0, comp, str)
#endif

#define INJ_SILENT(str...) do {} while(0)
#define INJ_TRACE(str...) TRACE(INJECTOR, str)
#define INJ_VERBOSE(str...) VERBOSE(INJECTOR, str)
#define INJ_WARNING(str...) WARNING(INJECTOR, str)
#define INJ_WARNING_CONT(str...) WARNING_CONT(INJECTOR, str)
#define INJ_ERROR(str...) ERROR(INJECTOR, str)
#define INJ_FATAL(str...) FATAL(INJECTOR, str)
#define INJ_FORCE(str...) FORCE(INJECTOR, str)
#define INJ_FORCE_CONT(str...) FORCE_CONT(INJECTOR, str)

#define ASSERT(c, str...)					\
	do { 									\
		if (!(c)) {							\
			INJ_FATAL("assertion failed\n");\
			INJ_FATAL(str);					\
			__exit(-1);						\
		}									\
	} while(0)

__END_DECLS


#endif
// vim:ts=4:sw=4

