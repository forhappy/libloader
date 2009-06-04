/* 
 * procutils.h
 * by WN @ Jun 04, 2009
 */

#ifndef PROCUTILS_H
#define PROCUTILS_H

#include <sys/cdefs.h>
#include <stdint.h>
#include <unistd.h>
#include "defs.h"

__BEGIN_DECLS

extern const char *
proc_get_file(pid_t pid, uintptr_t addr,
		char * buffer, int len);

extern void
proc_get_range(pid_t pid, const char * filename,
		uintptr_t * start, uintptr_t * end);

__END_DECLS

#endif

