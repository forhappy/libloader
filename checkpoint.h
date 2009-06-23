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

__BEGIN_DECLS

#ifndef INJECTOR
# define SCOPE
# define __open open
# define __close close
# define __read read
# define __write write
# define __printf printf
# define __exit exit
#endif


struct state_vector {
	
};

struct syscall_regs {
	uint32_t orig_eax;
	uint32_t eax;
	uint32_t ebx;
	uint32_t ecx;
	uint32_t edx;
	uint32_t esi;
	uint32_t edi;
	uint32_t ebp;
};

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

