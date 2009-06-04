/* 
 * ptraceutils.h
 * by WN # Jun. 04, 2009
 */

#ifndef PTRACEUTIILS_H
#define PTRACEUTIILS_H

#include <sys/cdefs.h>
#include <stdint.h>
#include <unistd.h>
#include "defs.h"

__BEGIN_DECLS

extern pid_t
ptrace_execve(const char * filename);

extern void
ptrace_dupmem(uint8_t * dst, uintptr_t addr, int len);

extern void
ptrace_updmem(uint8_t * dst, uintptr_t addr, int len);

/* Only 1 bkpt can exist */
extern void
ptrace_insert_bkpt(uintptr_t addr);

__END_DECLS
#endif


