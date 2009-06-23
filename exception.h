/* 
 * exception.h
 * by WN @ Mar. 07, 2009
 *
 * C exception facility, inspired by GDB's code.
 * This C exception is not suitable for dynamic objects. 
 * I use this exception handler to link all clean-ups together,
 * and to suitable recovery when needed.
 */

#ifndef CURRF2_EXCEPTION_H
#define CURRF2_EXCEPTION_H

#include <config.h>

#include <sys/cdefs.h>
#include <memory.h>
#include <stdlib.h>
#include <assert.h>
#include <setjmp.h>
#include <stdint.h>
#include <errno.h>

#include "defs.h"
#include "debug.h"
#include "list.h"
__BEGIN_DECLS


enum exception_level {
	EXCEPTION_NO_ERROR			= 0,
	EXCEPTION_USER_QUIT			= 1,
	EXCEPTION_SUBSYS_RERUN		= 2,
	EXCEPTION_SUBSYS_SKIPFRAME	= 3,
	EXCEPTION_SUBSYS_REINIT		= 4,
	EXCEPTION_SYS_RERUN			= 5,
	EXCEPTION_SYS_SKIPFRAME		= 6,
	EXCEPTION_SYS_REINIT		= 7,
	EXCEPTION_FATAL				= 8,
	EXCEPTION_RESET				= 9,
	EXCEPTION_CONTINUE			= 10,
	EXCEPTION_RESOURCE_LOST		= 11,
	EXCEPTION_RENDER_ERROR		= 12,
};

#define MASK(level)				(1 << (int)(level))
#define MASK_QUIT				MASK(EXCEPTION_USER_QUIT)
#define MASK_SUBSYS_RERUN		MASK(EXCEPTION_SUBSYS_RERUN)
#define MASK_SUBSYS_SKIPFRAME	MASK(EXCEPTION_SUBSYS_SKIPFRAME)
#define MASK_SUBSYS_REINIT		MASK(EXCEPTION_SUBSYS_REINIT)
#define MASK_SYS_RERUN			MASK(EXCEPTION_SYS_RERUN)
#define MASK_SYS_SKIPFRAME		MASK(EXCEPTION_SYS_SKIPFRAME)
#define MASK_SYS_REINIT			MASK(EXCEPTION_SYS_REINIT)
#define MASK_FATAL				MASK(EXCEPTION_FATAL)
#define MASK_RESET				MASK(EXCEPTION_RESET)
#define MASK_CONTINUE			MASK(EXCEPTION_CONTINUE)
#define MASK_ALL				(0xffffffff)
#define MASK_NONFATAL			((MASK_ALL) & (~(MASK_FATAL)) &(~(MASK_RESET)))
#define MASK_SYS_ALL			(MASK_SYS_RERUN | MASK_SYS_SKIPFRAME | MASK_SYS_REINIT)
#define MASK_SUBSYS_ALL			(MASK_SUBSYS_RERUN | MASK_SUBSYS_SKIPFRAME | MASK_SUBSYS_REINIT)

#define MASK_RESOURCE_LOST		MASK(EXCEPTION_RESOURCE_LOST)
#define MASK_RENDER_ERROR		MASK(EXCEPTION_RENDER_ERROR)

#define EXCEPTION_MSG_MAXLEN	(256)
struct exception {
	enum exception_level level;
	char message[EXCEPTION_MSG_MAXLEN];
	/* helper ptr */
	uintptr_t val;
	const char * file;
	const char * func;
	int line;
};

#if defined(HAVE_SIGSETJMP)
#define EXCEPTIONS_SIGJMP_BUF		sigjmp_buf
#define EXCEPTIONS_SIGSETJMP(buf)	sigsetjmp((buf), 1)
#define EXCEPTIONS_SIGLONGJMP(buf,val)	siglongjmp((buf), (val))
#else
#define EXCEPTIONS_SIGJMP_BUF		jmp_buf
#define EXCEPTIONS_SIGSETJMP(buf)	setjmp(buf)
#define EXCEPTIONS_SIGLONGJMP(buf,val)	longjmp((buf), (val))
#endif

enum catcher_state {
	CATCHER_CREATED,
	CATCHER_RUNNING,
	CATCHER_RUNNING_1,
	CATCHER_ABORTING,
};

enum catcher_action {
	CATCH_ITER,
	CATCH_ITER_1,
	CATCH_THROWING,
};

struct cleanup {
	struct list_head list;
	struct list_head * chain;
	/* Cleanup func need a param, because sometime the struct cleanup
	 * is dynamically alloced, and need to be free, sometime it is static alloced. */
	void (*function)(struct cleanup * cleanup);
	void * args;
};

#define CLEANUP(c)	do {remove_cleanup((c)); (c)->function((c));} while(0)

struct catcher {
	EXCEPTIONS_SIGJMP_BUF buf;
	enum catcher_state state;
	volatile struct exception * exception;
	uint32_t mask;
	struct list_head cleanup_chain;
	struct list_head * saved_cleanup_chain;
	struct catcher * prev;
};

typedef uint32_t return_mask;

/* We left an unmatched bracket here deliberate. We MUST
 * keep the 'catcher' var. */
#define TRY_CATCH(EXCEPTION,MASK) \
     { \
		 struct catcher catcher; \
		 memset(&catcher, '\0', sizeof(catcher));\
       EXCEPTIONS_SIGJMP_BUF *buf = \
		 exceptions_state_mc_init (&catcher, &(EXCEPTION), (MASK)); \
       EXCEPTIONS_SIGSETJMP (*buf); \
     while (exceptions_state_mc_action_iter ()) \
       while (exceptions_state_mc_action_iter_1 ())

#define END_TRY	}

#define CATCH(exp)	END_TRY; switch (exp.level)

/* 
 * NOTICE: cleanup MUST inactived. see cleanup_actived
 */
void
make_cleanup(struct cleanup * cleanup);

void
remove_cleanup(struct cleanup * cleanup);

/*  move this cleanup to outer catcher */
void
push_cleanup(struct cleanup * cleanup);

/* move this cleanup to this catcher */
void
grab_cleanup(struct cleanup * cleanup);

/* move this cleanup to the 'outest' catchup */
void
throw_cleanup(struct cleanup * cleanup);

static inline int
is_cleanup_actived(struct cleanup * cleanup)
{
	if (list_head_deleted(&cleanup->list))
		return FALSE;
	if (list_empty(&cleanup->list))
		return FALSE;
	return TRUE;
}

void
do_cleanup(void);

EXCEPTIONS_SIGJMP_BUF *
exceptions_state_mc_init(
		struct catcher * catcher,
		volatile struct exception * exception,
		return_mask mask);

int
exceptions_state_mc_action_iter(void);

int
exceptions_state_mc_action_iter_1(void);

void
print_exception(enum debug_level l, enum debug_component c,
		struct exception exp);


NORETURN void
throw_exception(enum exception_level,
		uintptr_t val, const char * file, const char * func,
		int line, const char * fmt, ...) ATTR_NORETURN ATTR(format(printf, 6, 7));

#define THROW(l, m...)			throw_exception(l, 0, __FILE__, __FUNCTION__, __LINE__, m)
#define THROW_VAL(l, v, m...)	throw_exception(l, v, __FILE__, __FUNCTION__, __LINE__, m)
#define RETHROW(e)			throw_exception((e).level, (e).val, \
		(e).file, (e).func, (e).line, (char*)&((e).message))

#define NOTHROW(fn, ...)	do {		\
	volatile struct exception exp;		\
	TRY_CATCH(exp, MASK_NONFATAL) {		\
		fn(__VA_ARGS__);				\
	} END_TRY;							\
} while(0)

#define NOTHROW_RET(fn, defret, ...)	({	\
	typeof(fn(__VA_ARGS__)) retval;		\
	retval = defret;					\
	volatile struct exception exp;		\
	TRY_CATCH(exp, MASK_NONFATAL) {		\
		retval = fn(__VA_ARGS__);		\
	} END_TRY;							\
	retval;								\
})

#define TRY_CLEANUP(fn, cleanup, ...)	do {		\
	volatile struct exception exp;		\
	TRY_CATCH(exp, MASK_NONFATAL) {		\
		fn(__VA_ARGS__);				\
	} END_TRY;							\
	switch (exp.level) {				\
		case EXCEPTION_NO_ERROR:		\
			break;						\
		default:						\
			cleanup;					\
			RETHROW(exp);				\
	}									\
} while(0)

#define TRY_CLEANUP_RET(fn, defret, cleanup, ...)	({\
	typeof(fn(__VA_ARGS__)) retval;		\
	volatile struct exception exp;		\
	TRY_CATCH(exp, MASK_NONFATAL) {		\
		retval = fn(__VA_ARGS__);				\
	} END_TRY;							\
	switch (exp.level) {				\
		case EXCEPTION_NO_ERROR:		\
			break;						\
		default:						\
			cleanup;					\
			RETHROW(exp);				\
	}									\
	retval;								\
})

#define TRY_FINALLY(fn, fin, ...)	do {		\
	volatile struct exception exp;		\
	TRY_CATCH(exp, MASK_NONFATAL) {		\
		fn(__VA_ARGS__);				\
	} END_TRY;							\
	fin;						\
	switch (exp.level) {				\
		case EXCEPTION_NO_ERROR:		\
			break;						\
		default:						\
			RETHROW(exp);				\
	}									\
} while(0)

#define TRY_FINALLY_RET(fn, defret, fin, ...)	({\
	typeof(fn(__VA_ARGS__)) retval;		\
	volatile struct exception exp;		\
	TRY_CATCH(exp, MASK_NONFATAL) {		\
		retval = fn(__VA_ARGS__);				\
	} END_TRY;							\
	fin;						\
	switch (exp.level) {				\
		case EXCEPTION_NO_ERROR:		\
			break;						\
		default:						\
			RETHROW(exp);				\
	}									\
	retval;								\
})



#define THROWS(...)

/* pop all catcher, cleanup all then exit */
NORETURN void
fatal_cleanup(void) ATTR_NORETURN;

/* after subsystem reinit, exec the reinit hook to notify
 * the sensable objects. */
struct reinit_hook {
	struct list_head list;
	void (*fn)(struct reinit_hook *);
	void * pprivate;
};


#define assert_throw(x, msg...)	do { \
	if (!(x))	\
		THROW(EXCEPTION_FATAL, msg);\
} while(0)

#define assert_errno_throw(msg...) assert_throw((errno == 0), msg)


#define ETHROW(x...)	assert_errno_throw(x)
#define CTHROW(c, x...) assert_throw((c), x)

__END_DECLS


#endif
// vim:tabstop=4:shiftwidth=4

