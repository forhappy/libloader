/* 
 * ptraceutils.h
 * by WN # Jun. 04, 2009
 */

#ifndef PTRACEUTIILS_H
#define PTRACEUTIILS_H

#include <sys/cdefs.h>
#include <linux/user.h>
#include <stdint.h>
#include <unistd.h>
#include "defs.h"

__BEGIN_DECLS

extern pid_t
ptrace_execve(const char * filename, char ** argv);

extern void
ptrace_dupmem(void * dst, uintptr_t addr, int len);

extern void
ptrace_updmem(void * src, uintptr_t addr, int len);

extern void
ptrace_callfunc(uintptr_t addr, int nr_args, ...);

/* Only 1 bkpt can exist */
extern void
ptrace_insert_bkpt(uintptr_t addr);

extern void
ptrace_kill(void);

extern void
ptrace_detach(bool_t wait);

struct user_regs_struct
ptrace_peekuser(void);

void
ptrace_pokeuser(struct user_regs_struct s);
__END_DECLS
#endif


