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
#include "pthread_defs.h"

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int32_t s32;

typedef s32		compat_timer_t;
typedef s32		compat_int_t;
typedef	u32		compat_uptr_t;
typedef s32		compat_clock_t;

typedef u32		__u32;
typedef s32		__s32;

typedef union compat_sigval {
	compat_int_t	sival_int;
	compat_uptr_t	sival_ptr;
} compat_sigval_t;


typedef struct compat_siginfo{
	int si_signo;
	int si_errno;
	int si_code;

	union {
		int _pad[((128/sizeof(int)) - 3)];

		/* kill() */
		struct {
			unsigned int _pid;	/* sender's pid */
			unsigned int _uid;	/* sender's uid */
		} _kill;

		/* POSIX.1b timers */
		struct {
			compat_timer_t _tid;	/* timer id */
			int _overrun;		/* overrun count */
			compat_sigval_t _sigval;	/* same as below */
			int _sys_private;	/* not to be passed to user */
			int _overrun_incr;	/* amount to add to overrun */
		} _timer;

		/* POSIX.1b signals */
		struct {
			unsigned int _pid;	/* sender's pid */
			unsigned int _uid;	/* sender's uid */
			compat_sigval_t _sigval;
		} _rt;

		/* SIGCHLD */
		struct {
			unsigned int _pid;	/* which child */
			unsigned int _uid;	/* sender's uid */
			int _status;		/* exit code */
			compat_clock_t _utime;
			compat_clock_t _stime;
		} _sigchld;

		/* SIGILL, SIGFPE, SIGSEGV, SIGBUS */
		struct {
			unsigned int _addr;	/* faulting insn/memory ref. */
		} _sigfault;

		/* SIGPOLL */
		struct {
			int _band;	/* POLL_IN, POLL_OUT, POLL_MSG */
			int _fd;
		} _sigpoll;
	} _sifields;
} compat_siginfo_t;


typedef struct sigaltstack_ia32 {
	unsigned int	ss_sp;
	int		ss_flags;
	unsigned int	ss_size;
} stack_ia32_t;

struct sigcontext_ia32 {
       unsigned short gs, __gsh;
       unsigned short fs, __fsh;
       unsigned short es, __esh;
       unsigned short ds, __dsh;
       unsigned int di;
       unsigned int si;
       unsigned int bp;
       unsigned int sp;
       unsigned int bx;
       unsigned int dx;
       unsigned int cx;
       unsigned int ax;
       unsigned int trapno;
       unsigned int err;
       unsigned int ip;
       unsigned short cs, __csh;
       unsigned int flags;
       unsigned int sp_at_signal;
       unsigned short ss, __ssh;
       unsigned int fpstate;		/* really (struct _fpstate_ia32 *) */
       unsigned int oldmask;
       unsigned int cr2;
};

typedef u32               compat_sigset_word;

#define _COMPAT_NSIG		64
#define _COMPAT_NSIG_BPW	32
#define _COMPAT_NSIG_WORDS	(_COMPAT_NSIG / _COMPAT_NSIG_BPW)

typedef struct {
	compat_sigset_word	sig[_COMPAT_NSIG_WORDS];
} compat_sigset_t;

struct ucontext_ia32 {
	unsigned int	  uc_flags;
	unsigned int 	  uc_link;
	stack_ia32_t	  uc_stack;
	struct sigcontext_ia32 uc_mcontext;
	compat_sigset_t	  uc_sigmask;	/* mask last for extensibility */
};

struct _fpreg {
	unsigned short significand[4];
	unsigned short exponent;
};

struct _fpxreg {
	unsigned short significand[4];
	unsigned short exponent;
	unsigned short padding[3];
};

struct _xmmreg {
	unsigned long element[4];
};

struct _fpstate_ia32 {
	/* Regular FPU environment */
	__u32 	cw;
	__u32	sw;
	__u32	tag;	/* not compatible to 64bit twd */
	__u32	ipoff;			
	__u32	cssel;
	__u32	dataoff;
	__u32	datasel;
	struct _fpreg	_st[8];
	unsigned short	status;
	unsigned short	magic;		/* 0xffff = regular FPU data only */

	/* FXSR FPU environment */
	__u32	_fxsr_env[6];
	__u32	mxcsr;
	__u32	reserved;
	struct _fpxreg	_fxsr_st[8];
	struct _xmmreg	_xmm[8];	/* It's actually 16 */ 
	__u32	padding[56];
};


struct rt_sigframe
{
	u32 pretcode;
	int sig;
	u32 pinfo;
	u32 puc;
	compat_siginfo_t info;
	struct ucontext_ia32 uc;
	struct _fpstate_ia32 fpstate;
	char retcode[8];
};


extern void __vsyscall();


extern SCOPE char logger_filename[64];
extern SCOPE char ckpt_filename[64];

void SCOPE
do_wrapped_rt_sigreturn(struct rt_sigframe frame)
{
	INJ_WARNING("target received rt_signal %d, the log will switch\n", frame.sig);
	return;
}


void SCOPE
do_wrapped_sigreturn(void)
{
	INJ_WARNING("sigreturn\n");
	INTERNAL_SYSCALL(exit, 1, 10);
	return;
}

// vim:ts=4:sw=4

