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
#include <asm/unistd.h>
#include "defs.h"

__BEGIN_DECLS

extern pid_t
ptrace_execve(const char * filename, char ** argv);

extern void
ptrace_dupmem(void * dst, uintptr_t addr, int len);

extern void
ptrace_updmem(const void * src, uintptr_t addr, int len);

extern void
ptrace_callfunc(uintptr_t addr, int nr_args, ...);

/* Only 1 bkpt can exist */
extern void
ptrace_insert_bkpt(uintptr_t addr);

extern uintptr_t
ptrace_next_syscall(int (*hook)(struct user_regs_struct u, bool_t before));

extern void
ptrace_kill(void);

extern void
ptrace_detach(bool_t wait);

struct user_regs_struct
ptrace_peekuser(void);

void
ptrace_pokeuser(struct user_regs_struct s);

uintptr_t
ptrace_cont(void);

void
ptrace_resume(void);

void
ptrace_goto(uintptr_t addr);

/* return the esp */
uint32_t
ptrace_push(const void * data, int len, bool_t save_esp);

#define syscallno(x) __NR_##x

uint32_t
ptrace_syscall(int no, int nr, ...);

#define ptrace_syscall(x, nr...)	ptrace_syscall(syscallno(x), nr)

__END_DECLS
#endif


