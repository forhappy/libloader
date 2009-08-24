/* 
 * newprocess.c
 * by WN @ Aug. 10, 2009
 */

#include "syscalls.h"
#include <unistd.h>
#include <fcntl.h> /* for open */
#include "signal_defs.h"	/* for unhook_sighandlers */

#define CSIGNAL		0x000000ff	/* signal mask to be sent at exit */
#define CLONE_VM	0x00000100	/* set if VM shared between processes */
#define CLONE_FS	0x00000200	/* set if fs info shared between processes */
#define CLONE_FILES	0x00000400	/* set if open files shared between processes */
#define CLONE_SIGHAND	0x00000800	/* set if signal handlers and blocked signals shared */
#define CLONE_PTRACE	0x00002000	/* set if we want to let tracing continue on the child too */
#define CLONE_VFORK	0x00004000	/* set if the parent wants the child to wake it up on mm_release */
#define CLONE_PARENT	0x00008000	/* set if we want to have the same parent as the cloner */
#define CLONE_THREAD	0x00010000	/* Same thread group? */
#define CLONE_NEWNS	0x00020000	/* New namespace group? */
#define CLONE_SYSVSEM	0x00040000	/* share system V SEM_UNDO semantics */
#define CLONE_SETTLS	0x00080000	/* create a new TLS for the child */
#define CLONE_PARENT_SETTID	0x00100000	/* set the TID in the parent */
#define CLONE_CHILD_CLEARTID	0x00200000	/* clear the TID in the child */
#define CLONE_DETACHED		0x00400000	/* Unused, ignored */
#define CLONE_UNTRACED		0x00800000	/* set if the tracing process can't force CLONE_PTRACE on this clone */
#define CLONE_CHILD_SETTID	0x01000000	/* set the TID in the child */
#define CLONE_STOPPED		0x02000000	/* Start in stopped state */
#define CLONE_NEWUTS		0x04000000	/* New utsname group? */
#define CLONE_NEWIPC		0x08000000	/* New ipcs */
#define CLONE_NEWUSER		0x10000000	/* New user namespace */
#define CLONE_NEWPID		0x20000000	/* New pid namespace */
#define CLONE_NEWNET		0x40000000	/* New network namespace */
#define CLONE_IO		0x80000000	/* Clone io context */

extern void __vsyscall();

extern SCOPE char logger_filename[128];
extern SCOPE char ckpt_filename[128];
extern SCOPE pid_t self_pid;
extern SCOPE pid_t old_self_pid;
/* this is a aligned struct. although pass NULL as fpustate is OK,
 * use this str can faster. */
extern SCOPE struct i387_fxsave_struct fpustate_struct;

static inline void
checkflags(unsigned long clone_flags, const struct syscall_regs * regs)
{
	unsigned long not_support =
		CLONE_VM | CLONE_SIGHAND | CLONE_PTRACE | CLONE_VFORK |
		CLONE_PARENT | CLONE_THREAD | CLONE_NEWNS | CLONE_SYSVSEM |
		CLONE_SETTLS | CLONE_IO;
	ASSERT(!(clone_flags & not_support), regs, "clone_flags 0x%x contain non-support flags\n");
}

extern SCOPE int tracing;

SCOPE void
do_child_fork(unsigned long clone_flags,
		unsigned long stack_start,
		const struct syscall_regs * regs,
		uintptr_t parent_tidptr,
		uintptr_t child_tidptr)
{
	int err;

	INJ_TRACE("do child fork\n");
	checkflags(clone_flags, regs);

	/* close current logger in child */
	err = INTERNAL_SYSCALL(close, 1, logger_fd);
	ASSERT(err == 0, regs, "close current logger %s failed\n", logger_filename);

	if (!injector_opts.trace_fork) {
		/* don't trace child, no checkpoint and log */
		/* we need to remove sigaction's restorer */
		unhook_sighandlers();
		tracing = 0;
		return;
	}

	self_pid = old_self_pid = INTERNAL_SYSCALL(getpid, 0);

	/* reset logger_filename and ckpt_filename to indicate that there are no
	 * old ckpt and log */
	logger_filename[0] = '\0';
	ckpt_filename[0] = '\0';

	struct syscall_regs r;
	memcpy(&r, regs, sizeof(r));

	/* reset esp */
	r.esp = stack_start;
	/* don't touch eip */
	save_i387(&fpustate_struct);
	make_checkpoint(&r, &fpustate_struct, NULL, TRUE);
}

SCOPE void
do_parent_fork(unsigned long clone_flags,
		unsigned long stack_start,
		const struct syscall_regs * regs,
		uintptr_t parent_tidptr,
		uintptr_t child_tidptr)
{
	checkflags(clone_flags, regs);
	/* do nothing */
	return;
}

