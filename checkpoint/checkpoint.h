/* 
 * recorder.h
 * by WN @ Jun. 22, 2009
 */

#ifndef __CHECKPOINT_H
#define __CHECKPOINT_H

#include <sys/cdefs.h>
#include <stdint.h>
#include <unistd.h>
#include "defs.h"
#include "syscall_table.h"

#ifndef INJECTOR
# include "ptraceutils.h"
# include <stdio.h>
# include "debug.h"
# include "exception.h"
#endif

__BEGIN_DECLS

#ifndef INJECTOR
# define SCOPE
# define __open open
# define __close close
# define __read read
# define __write write
# define __printf printf
# define __exit(x) THROW(EXCEPTION_FATAL, "checkpoint exit")
# define __dup_mem(d, s, sz) ptrace_dupmem(d, s, sz)
#endif

extern SCOPE struct state_vector {
	int dummy;
	uint32_t brk;
} state_vector;

extern SCOPE int
checkpoint_init(void);

extern SCOPE int
logger_init(pid_t pid);

extern SCOPE int
logger_close(void);

extern SCOPE int
before_syscall(struct syscall_regs regs);

extern SCOPE int
after_syscall(struct syscall_regs regs);

__END_DECLS

#endif

