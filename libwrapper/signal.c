/* 
 * signal.c
 * by WN @ Aug. 06, 2009
 */

#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>

#include "injector.h"
#include "checkpoint/checkpoint.h"
#include "injector_utils.h"
#include "injector_debug.h"

#include "signal_defs.h"

extern void __vsyscall();


extern SCOPE char logger_filename[128];
extern SCOPE char ckpt_filename[128];
extern SCOPE pid_t self_pid;

/* this var is inited each time we enter a syscall */
extern const struct syscall_regs * current_regs;

/* defined in injector.h */
/* extern SCOPE volatile enum syscall_status syscall_status; */

static void
do_sigreturn(int signum, struct sigcontext * ctx, struct _fpstate * fpstate)
{
	int err;
	/* generate timestamp for new ckpt */
	struct timeval tv;
	err = INTERNAL_SYSCALL(gettimeofday, 2, &tv, NULL);
	ASSERT(err == 0, "gettimeofday failed: %d\n", err);

	snprintf(ckpt_filename, 128, LOGGER_DIRECTORY"/%d-%u-%u.ckpt",
			self_pid, tv.tv_sec, tv.tv_usec);

	/* before we close current logger, we write a int16_t -signum into logger.
	 * this num should become last 2 bytes in logger.
	 * see comment in wrapper.c - wrapped_syscall */
	{
		int16_t f = -signum;
		INTERNAL_SYSCALL(write, 3, logger_fd, &f, sizeof(f));
	}

	/* close current logger */
	err = INTERNAL_SYSCALL(close, 1, logger_fd);
	ASSERT(err == 0, "close logger file %s failed\n", logger_filename);

	/* write logger_filename before we make ckpt, so when replay
	 * target will get correct logger filename */
	snprintf(logger_filename, 128, LOGGER_DIRECTORY"/%d-%u-%u.log",
			self_pid, tv.tv_sec, tv.tv_usec);

	struct syscall_regs r;
	memset(&r, '\0', sizeof(r));

	/* we don't use context's regs in checkpoint, if the signal
	 * breaks a syscall.
	 *
	 * think the signal handling, check and replay:
	 *
	 * if we use the context's regs, then ip should
	 * point at the next instr of 'sysenter', or 'sysenter' if
	 * we use SA_RESTART and such syscall return some -ERESTART_XXX.
	 * then when we run gdbloader, the checkpoint will return to
	 * wrapped_syscall, and everything become inconsistent.
	 *
	 * here we use the current_regs (set each time when it go into a syscall).
	 * then in replay, we start just before the syscalling.
	 * we have to choise:
	 *
	 * 1. glibc always use a 'call   *%gs:0x10' to enter a syscall, which
	 *    is '65 ff 15 10 00 00 00'. we sub the eip by 7, make it point to
	 *    the 'call' instr. we need to add esp by 4 for the 'retcode'.
	 * 2. we point eip to the syscall_wrapper_entry. we needn't any stack
	 *    adjustment. this is easy, but when gdb attached, user don't know
	 *    the call stack, and this method need relocation.
	 * */
	if (syscall_status == OUT_OF_SYSCALL) {
		INJ_WARNING("break no syscall\n");
#define copy_reg(x)	r.e##x = ctx->x
		copy_reg(ax);
		copy_reg(bx);
		copy_reg(cx);
		copy_reg(dx);
		copy_reg(si);
		copy_reg(di);
		copy_reg(bp);
		copy_reg(sp);
		copy_reg(ip);
#undef copy_reg
		r.flags = ctx->flags;
		r.orig_eax = 0;
	} else {
		INJ_WARNING("break syscall\n");
		memcpy(&r, current_regs, sizeof(r));
		r.eip -= 7;
		r.esp += 4;
		r.orig_eax = ctx->ax;
	}

	/* build fake regs and fpustate */
	struct seg_regs seg_regs;
#define copy_sreg(x)	seg_regs.x = ctx->x
	copy_sreg(cs);
	copy_sreg(ss);
	copy_sreg(ds);
	copy_sreg(es);
	copy_sreg(fs);
	copy_sreg(gs);
#undef copy_sreg

	/* from kernel's code: arch/x86/kernel/signal_32.c, restore_sigcontext,
	 * frame.fpstate is actually struct i387_fxsave_struct, although their
	 * size is defferent. */
	make_checkpoint(ckpt_filename, &r,
			(struct i387_fxsave_struct*)(fpstate), &seg_regs);

	/* create new logger file */
	int fd;
	fd = INTERNAL_SYSCALL(open, 3, logger_filename, O_WRONLY|O_APPEND|O_CREAT, 0666);
	ASSERT(fd > 0, "open new logger file %s failed: %d\n", logger_filename, fd);

	/* the old logger should have been closed */
	err = INTERNAL_SYSCALL(dup2, 2, fd, LOGGER_FD);
	ASSERT(err == LOGGER_FD, "dup2 failed: %d\n", err);
	INJ_TRACE("dup fd to %d\n", err);

	err = INTERNAL_SYSCALL(close, 1, fd);
	ASSERT(err == 0, "close failed: %d\n", err);
	INJ_TRACE("close %d\n", fd);

	logger_fd = LOGGER_FD;

	/* we reset syscall_status to SIGNALED, make the syscall wrapper know there's a signal
	 * distrub current syscall */
	syscall_status = SIGNALED;
}


void SCOPE
do_wrapped_rt_sigreturn(struct rt_sigframe frame)
{
	INJ_WARNING("target received rt_signal %d, the log will be switched\n", frame.sig);
	do_sigreturn(frame.sig, &frame.uc.uc_mcontext, &frame.fpstate);
	INJ_WARNING("return from do_wrapped_rt_sigreturn\n");

	return;
}


void SCOPE
do_wrapped_sigreturn(struct sigframe frame)
{
	INJ_WARNING("target received signal %d, the log will be switched\n", frame.sig);
	do_sigreturn(frame.sig, &frame.sc, &frame.fpstate);
	INJ_WARNING("return from do_wrapped_sigreturn\n");
	return;
}

// vim:ts=4:sw=4

