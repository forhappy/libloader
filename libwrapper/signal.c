/* FIXME check for tracing... */

/* 
 * signal.c
 * by WN @ Aug. 14, 2009
 */

#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>

#include "injector.h"
#include "checkpoint/checkpoint.h"
#include "injector_utils.h"
#include "injector_debug.h"

#include "signal_defs.h"

extern SCOPE char logger_filename[128];
extern SCOPE char ckpt_filename[128];
extern SCOPE pid_t self_pid;

/* this var is inited each time we enter a syscall */
extern const struct syscall_regs * signal_regs;

extern void wrapped_rt_sigreturn(void);
extern void wrapped_sigreturn(void);
extern void wrapped_sighandler(void);

void SCOPE
unhook_sighandlers(void)
{
	struct k_sigaction * acts = state_vector.sigactions;
	/* for each signal, we remove the SA_RESTORER flag and reset it */
	for (int i = 0; i < K_NSIG; i++) {
		int err;
		struct k_sigaction act;
		err = INTERNAL_SYSCALL(rt_sigaction, 4,
				i, NULL, &act, sizeof(act));
		ASSERT(err != 0, NULL, "rt_sigaction %d failed\n", i);
		if (act.sa_handler == wrapped_sighandler) {
			act.sa_handler = acts[i].sa_handler;
			err = INTERNAL_SYSCALL(rt_sigaction, 4,
					i, &act, NULL, sizeof(act));
			ASSERT(err != 0, NULL, "rt_sigaction %d failed\n", i);
		}
	}
}

#define SA_SIGINFO	0x00000004u

static void
do_make_signal_ckpt(int signum, struct syscall_regs * regs)
{
	int err;
	/* generate timestamp for new ckpt */
	struct timeval tv;
	err = INTERNAL_SYSCALL(gettimeofday, 2, &tv, NULL);
	ASSERT(err == 0, NULL, "gettimeofday failed: %d\n", err);

	snprintf(ckpt_filename, 128, LOGGER_DIRECTORY"/%d-%u-%u.ckpt",
			self_pid, tv.tv_sec, tv.tv_usec);
	
	/* before we close current logger, we write a int16_t -signum into logger.
	 * this num should become last 2 bytes in logger.
	 * see comment in wrapper.c - wrapped_syscall */
	{
		int16_t f = - signum - 1;
		INTERNAL_SYSCALL(write, 3, logger_fd, &f, sizeof(f));
	}

	/* close current logger */
	err = INTERNAL_SYSCALL(close, 1, logger_fd);
	ASSERT(err == 0, NULL, "close logger file %s failed\n", logger_filename);

	/* write logger_filename before we make ckpt, so when replay
	 * target will get correct logger filename */
	snprintf(logger_filename, 128, LOGGER_DIRECTORY"/%d-%u-%u.log",
			self_pid, tv.tv_sec, tv.tv_usec);

	struct syscall_regs r;
	memset(&r, '\0', sizeof(r));

	/* we can use ctx's regs directly. */
	r = *regs;

	make_checkpoint(ckpt_filename, &r,
			NULL, NULL);

	/* create new logger file */
	int fd;
	fd = INTERNAL_SYSCALL(open, 3, logger_filename, O_WRONLY|O_APPEND|O_CREAT, 0664);
	ASSERT(fd > 0, &r, "open new logger file %s failed: %d\n", logger_filename, fd);

	/* the old logger should have been closed */
	err = INTERNAL_SYSCALL(dup2, 2, fd, LOGGER_FD);
	ASSERT(err == LOGGER_FD, &r, "dup2 failed: %d\n", err);
	INJ_TRACE("dup fd to %d\n", err);

	err = INTERNAL_SYSCALL(close, 1, fd);
	ASSERT(err == 0, &r, "close failed: %d\n", err);
	INJ_TRACE("close %d\n", fd);

	logger_fd = LOGGER_FD;

	/* logger_sz has been reset in do_make_checkpoint */
	logger_sz = 0;

	INJ_WARNING("process receive signal %d, a new ckpt %s is made\n",
			signum, ckpt_filename);
}

static void
make_signal_ckpt(void * f, struct k_sigaction * act,
		struct syscall_regs * regs)
{
	/* don't forget the eip here is useless in real running */
	regs->eip = (uint32_t)(act->sa_handler);
	if (act->sa_flags & SA_SIGINFO) {
		/* this is a rt_frame */
		struct rt_sigframe * frame = f;
		/* we clear the sigmask when making ckpt and restore it.
		 * don't block any signal when replay. */
		/* clear it when replay */
		k_sigset_t mask = frame->uc.uc_sigmask;
		memset(&frame->uc.uc_sigmask, 0, sizeof(k_sigset_t));
		do_make_signal_ckpt(
			frame->sig, regs);
		memcpy(&frame->uc.uc_sigmask, &mask, sizeof(k_sigset_t));
	} else {
		struct sigframe * frame = f;
		k_sigset_t mask;
#define oldm	(frame->sc.oldmask)
#define extm	(frame->extramask)
		memcpy(&mask.sig[0], &oldm, sizeof(oldm));
		memcpy(&mask.sig[1], &extm, sizeof(extm));
		memset(&oldm, 0, sizeof(oldm));
		memset(&extm, 0, sizeof(extm));
		do_make_signal_ckpt(
			frame->sig, regs);
		memcpy(&oldm, &mask.sig[0], sizeof(oldm));
		memcpy(&extm, &mask.sig[1], sizeof(extm));
#undef extm
#undef oldm
	}
}

static void
write_frame_to_log(void * f, struct k_sigaction * act,
	struct syscall_regs * regs)
{
#define write_frame(frame) do {	\
	write_int16(-frame->sig - 1);	\
	write_int32(sizeof(*frame));	\
	write_mem(frame, sizeof(*frame));		\
	write_mem(regs, sizeof(*regs));	\
	write_mem(signal_regs, sizeof(*signal_regs));	\
	write_mem(act, sizeof(*act));		\
} while(0)

	if (act->sa_flags & SA_SIGINFO) {
		struct rt_sigframe * frame = f;
		write_frame(frame);
	} else {
		struct sigframe * frame = f;
		write_frame(frame);
	}
#undef write_frame
}

struct per_frame_info{
	uintptr_t psyscall_regs;
	uint16_t syscall_nr;
	uint8_t is_break;
} ATTR(packed);

/* this function only called in running phase, not in replay phase. */
void SCOPE
do_wrapped_sighandler(volatile struct syscall_regs handler_regs,
		volatile uint32_t real_sighandler,
		volatile uint32_t pretcode,
		volatile int signum)
{
	INJ_WARNING("proc received signal %d\n", signum);
	struct k_sigaction * pact = &state_vector.sigactions[signum];
	struct per_frame_info * per_frame_info = NULL;

	real_sighandler = (uint32_t)(pact->sa_handler);

	/* we borrow the retcode (8 bytes, kernel's code say we shouldn't use
	 * it any more) to save pre-signal infos. */
	if (pact->sa_flags & SA_SIGINFO) {
		pretcode = (uint32_t)(wrapped_rt_sigreturn);
		struct rt_sigframe * frame = (void*)(&pretcode);
		per_frame_info = (void*)&frame->retcode;
	} else {
		pretcode = (uint32_t)(wrapped_sigreturn);
		struct sigframe * frame = (void*)(&pretcode);
		per_frame_info = (void*)&frame->retcode;
	}

	/* now, procmask has changes. */
	/* we should restore this state in sigreturn. then when following
	 * syscall DISABLE and ENABLE signal, mask works correctly. */
	/* we write this before we make ckpt. */
	k_sigset_t mask = pact->sa_mask;
	k_sigset_t * pm = &state_vector.sigmask;
	sigaddset(&mask, signum);
	sigorsets(pm, pm, &mask);

	/* check whether inside a syscall */
	if (IS_BREAK_SYSCALL()) {
		INJ_WARNING("syscall %d is broken by this signal: %d > %d\n",
				signal_regs->orig_eax,
				__syscall_reenter_counter,
				__syscall_reenter_base);
		per_frame_info->is_break = 1;
		/* the pointer of signal_regs is valid only in this func.
		 * but the content of signal_regs is on stack, valid
		 * in sigreturn. */
		per_frame_info->syscall_nr = signal_regs->orig_eax;
		per_frame_info->psyscall_regs = (uint32_t)signal_regs;

		/* all important info, such as retcode, has been written. */
		write_frame_to_log((void*)&pretcode, pact,
					(struct syscall_regs*)&handler_regs);
	} else {
		INJ_WARNING("no syscall is broken\n");
		/* make a ckpt as usual */
		per_frame_info->is_break = 0;
		make_signal_ckpt((void*)&pretcode, pact,
				(struct syscall_regs*)&handler_regs);
	}

	/* finally, we unlock signals */
	/* notice: we need to block this signal itself */
	/* reset sigmask to correct one */
	INTERNAL_SYSCALL(rt_sigprocmask, 4,
		SIG_SETMASK, pm, NULL, sizeof(k_sigset_t));
}


extern int SCOPE replay;

void SCOPE
do_wrapped_sigreturn(struct sigframe frame)
{
	if (!replay) {
		/* we restore sigprocmask */
		k_sigset_t mask;
		memcpy(&mask.sig[0], &frame.sc.oldmask, sizeof(frame.sc.oldmask));
		memcpy(&mask.sig[1], &frame.extramask, sizeof(frame.extramask));
		state_vector.sigmask = mask;
	}

	/* we do some check here */
	/* both running phase and replay phase can do this check,
	 * because the frame should have been rewritten by replayer. */
	struct per_frame_info * per_frame_info = (void*)(&frame.retcode);
	struct syscall_regs * regs = (void*)(per_frame_info->psyscall_regs);

	/* in the wrapped sigreturn, we need to redo before_syscall,
	 * write the header of broken syscall, and let the original code write the result. */

	if (!replay) {
		/* in running phase, we redo before_syscall */
		if (per_frame_info->is_break)
			before_syscall(regs);
		/* return normally */
	}
	/* in replay phase, return normally. frame should have been adjusted
	 * by loader */
	/* in running phase, when we do sigreturn, if the syscall is break, then we must
	 * restore the __syscall_reenter_counter and __syscall_reenter_base. sometime a syscall
	 * can be break more than once. when it break a 2nd time, those 2 flags has been override. */
	if (per_frame_info->is_break) {
		__syscall_reenter_base -= 1;
		/* also, signal_regs should be reset */
		signal_regs = regs;
	}
}

void SCOPE
do_wrapped_rt_sigreturn(struct rt_sigframe frame)
{
	if (!replay) {
		state_vector.sigmask = frame.uc.uc_sigmask;
	}

	struct per_frame_info * per_frame_info = (void*)(&frame.retcode);
	struct syscall_regs * regs =
		(struct syscall_regs *)per_frame_info->psyscall_regs;

	if (!replay) {
		if (per_frame_info->is_break)
			before_syscall(regs);
	}
	if (per_frame_info->is_break) {
		__syscall_reenter_base -= 1;
		signal_regs = regs;
	}
}

/* regs is used only for ASSERT */
void SCOPE ATTR(noreturn)
replay_sigaction(int signum, const struct syscall_regs * regs)
{
	INJ_FORCE("target receive signal %d, use sighandler\n", signum);
	/* replay_sigaction load sigframe, handler regs and syscall regs,
	 * build stack and adjust sigframe, then restore handler regs and
	 * ret to signal handler. never return. */

	/* signum has read */

	/* then size of frame */
	int32_t sz_frame = read_int32();

	union {
		struct rt_sigframe rf;
		struct sigframe f;
	} uf;

	struct sigcontext * ctx = NULL;
	if (sz_frame == sizeof(uf.rf)) {
		read_obj(uf.rf);
		ctx = &uf.rf.uc.uc_mcontext;
		/* clear sigmasks */
		memset(&uf.rf.uc.uc_sigmask, 0, sizeof(k_sigset_t));
	} else {
		ASSERT(sz_frame == sizeof(uf.f), regs, "corrupted log file: sz_frame = %d\n",
				sz_frame);
		read_obj(uf.f);
		ctx = &uf.f.sc;
		memset(&uf.f.extramask, 0, sizeof(uf.f.extramask));
	}
		/* clear sigmasks */
	ctx->oldmask = 0;
	
	struct syscall_regs handler_regs, _signal_regs;
	read_obj(handler_regs);
	read_obj(_signal_regs);

	struct k_sigaction act;
	read_obj(act);

	/* check _signal_regs */
	if (memcmp(&_signal_regs, signal_regs, sizeof(_signal_regs))) {
		INJ_WARNING("current regs is different from logged\n");
		/* at least the stack have to be same */
		ASSERT(_signal_regs.esp == signal_regs->esp, regs,
				"stack different, cannot continue\n");
	}

	/* adjust frame according to signal_regs */
	/* move the eip and esp to the status before call *%gs:0x10 */
	_signal_regs.eip -= 7;
	_signal_regs.esp += 4;
#define copy_reg(x) ctx->x = _signal_regs.e##x
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
	ctx->flags = _signal_regs.flags;
	
	/* build frame according to handler_regs */

	/* write frame to handler_regs.esp */
	__dup_mem(handler_regs.esp, &uf, sz_frame);

	/* push sighandler on it */
	void * stack_top = (void*)handler_regs.esp;
#define push_val(x)	do {		\
	stack_top -= sizeof(x);		\
	__dup_mem(stack_top, &x, sizeof(x));\
} while(0)
	push_val(act.sa_handler);
#undef push_val

	/* never return: */
	/* 1. push every regs onto current stact */
	/* 2. pop every regs, last one is esp */
	/* 3. ret. */
#define hr(x)	[x] "m" (handler_regs.x)
	asm volatile (
		"pushl	%[esp]\n"
		"pushl	%[flags]\n"
		"pushl	%[ebp]\n"
		"pushl	%[edi]\n"
		"pushl	%[esi]\n"
		"pushl	%[edx]\n"
		"pushl	%[ecx]\n"
		"pushl	%[ebx]\n"
		"pushl	%[eax]\n"
		"popl 	%%eax\n"
		"popl	%%ebx\n"
		"popl	%%ecx\n"
		"popl	%%edx\n"
		"popl	%%esi\n"
		"popl	%%edi\n"
		"popl	%%ebp\n"
		"popf\n"
		"popl	%%esp\n"	/* we never back... */
		"ret\n"
		:
		:
		[esp] "m" (stack_top),
		hr(flags), hr(ebp), hr(edi), hr(esi),
		hr(edx), hr(ecx), hr(ebx), hr(eax));
#undef hr
		/* never return */
		while(1);
}

