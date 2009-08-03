/* 
 * WN @ Jan 24, 2009
 */

/* Debug facility */

#include "config.h"

#include <stdarg.h>

#include <stdint.h>
#include <unistd.h>

#define CURRF2_DEBUG_C /* Debug masks */
#include "injector_debug.h"
#include "injector_utils.h"


#ifdef set_comp_level
#undef set_comp_level
#endif
void SCOPE
set_comp_level(enum debug_component comp, enum debug_level level)
{
	debug_levels[comp] = level;
}

#ifdef get_comp_level
#undef get_comp_level
#endif
SCOPE enum debug_level
get_comp_level(enum debug_component comp)
{
	return debug_levels[comp];
}


static const char *
get_comp_name(enum debug_component comp)
{
	return debug_comp_name[comp];
}

static const char *
get_level_name(enum debug_level level)
{
	static char * debug_level_names[NR_DEBUG_LEVELS] = {
#ifdef CURRF2_DEBUG_OFF
		[SILENT] 	= "SILENT",
		[TRACE]		= "TRACE",
		[VERBOSE]	= "VERBOSE",
		[WARNING]	= "WARNING",
		[ERROR]		= "ERROR",
		[FATAL]		= "FATAL",
		[FORCE]		= "FORCE",
#else
		[SILENT] 	= "S",
		[TRACE]		= "T",
		[VERBOSE]	= "V",
		[WARNING]	= "WW",
		[ERROR]		= "EW",
		[FATAL]		= "FW",
		[FORCE]		= "FORCE",
#endif
	};
	if ((level >= 0) && (level < NR_DEBUG_LEVELS))
		return debug_level_names[level];
	else
		return "Unknown";
}

static void
turn_red(void)
{
	fdprintf(STDOUT_FILENO_INJ, "%c[1;31m", 0x1b);
}

static void
turn_blue(void)
{
	fdprintf(STDOUT_FILENO_INJ, "%c[1;34m", 0x1b);
}


static void
turn_normal(void)
{
	fdprintf(STDOUT_FILENO_INJ, "%c[m", 0x1b);
}

#ifdef debug_out
#undef debug_out
#endif

static void
vdebug_out(int prefix, enum debug_level level, enum debug_component comp,
	       const char * func_name, int line_no,
       	       const char * fmt, va_list ap)
{
	if (debug_levels[comp] <= level) {
		if (level >= WARNING) {
			if (level == FORCE)
				turn_blue();
			else
				turn_red();
		}
		if (prefix) {
			fdprintf(STDOUT_FILENO_INJ, "[%s %s@%s:%d]\t", get_comp_name(comp),
					get_level_name(level), func_name, line_no);
		}

		vfdprintf(STDOUT_FILENO_INJ, fmt, ap);

		if (level >= WARNING)
			turn_normal();
	}
}

void SCOPE
debug_out(int prefix, enum debug_level level, enum debug_component comp,
	       const char * func_name, int line_no,
       	       const char * fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	vdebug_out(prefix, level, comp, func_name, line_no,
			fmt, ap);
	va_end(ap);
}

#ifdef CURRF2_DEBUG_OFF
static void SCOPE
vmessage_out(int prefix, enum debug_level l, enum debug_component c, char * fmt, va_list ap)
{
	if (l >= WARNING)
		turn_red();

	if (prefix)
		fdprintf(STDOUT_FILENO_INJ, "%s: ", get_level_name(l));
	vfdprintf(STDOUT_FILENO_INJ, fmt, ap);

	if (l >= WARNING)
		turn_normal();
}

void SCOPE
message_out(int prefix, enum debug_level l, enum debug_component c, char * fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	vmessage_out(prefix, l, c, fmt, ap);
	va_end(ap);
}
#endif

// vim:tabstop=4:shiftwidth=4

