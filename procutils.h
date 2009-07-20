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

#define PE_START	(0x01)
#define PE_END		(0x02)
#define PE_ADDR		(0x04)
#define PE_FILE		(0x08)

struct proc_entry {
	uint32_t start;
	uint32_t end;
	uint32_t addr;
	char fn[256];
	uint32_t bits;

	/* other info */
	uint32_t prot;
	uint32_t offset;
};

extern void
proc_fill_entry(struct proc_entry * entry,
		pid_t pid);

extern const char *
proc_get_file(pid_t pid, uintptr_t addr,
		char * buffer, int len);

extern void
proc_get_range(pid_t pid, const char * filename,
		uintptr_t * pstart, uintptr_t * pend);

extern bool_t
proc_find_in_range(struct proc_entry * entry,
		pid_t pid, uintptr_t start, uintptr_t end);
__END_DECLS

#endif

