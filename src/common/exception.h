/* 
 * exception.h
 * by WN @ Nuv. 16, 2009
 */
/* 
 * support for TRY---CATCH---FINALLY.
 */

#ifndef __EXCEPTION_H
#define __EXCEPTION_H

#include <config.h>
#include <common/defs.h>
#include <stdint.h>
#include <setjmp.h>


__BEGIN_DECLS

enum exception_type {
	EXP_NO_EXCEPTION,
#define def_exp_type(a, b)	a,
#include <common/exception_types.h>
#undef def_exp_type
	EXP_UNCATCHABLE,
};

#define EXCEPTION_MSG_LEN_MAX	(256)

struct exception_t {
	enum exception_type type;
	char msg[EXCEPTION_MSG_LEN_MAX];
	union {
		int val;
		void * ptr;
		uintptr_t xval;
	} u;
#ifdef SNITCHASER_DEBUG
	const char * file;
	const char * func;
	int line;
#endif
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

enum catcher_action {
	CATCH_ITER_0,
	CATCH_FINALLY,
	CATCH_THROW,
};
enum catcher_state {
	CATCHER_INIT,
	CATCHER_EXEC_BODY,
	CATCHER_WAIT_FINALLY,
	CATCHER_THROWN,
};

struct catcher_t {
	EXCEPTIONS_SIGJMP_BUF buf;
	enum catcher_state state;
	struct exception_t * curr_exp;
	struct catcher_t * prev;
};

extern bool_t
exceptions_state_mc(enum catcher_action);
extern EXCEPTIONS_SIGJMP_BUF *
exceptions_state_mc_init(struct catcher_t * catcher,
		struct exception_t * exp);

#define TRY(exp) \
	<% \
	struct catcher_t ____catcher; \
	____catcher.curr_exp = &(exp); \
	EXCEPTIONS_SIGJMP_BUF * buf = \
		exceptions_state_mc_init(&____catcher, &(exp));	\
	EXCEPTIONS_SIGSETJMP(*buf);	\
	while (exceptions_state_mc(CATCH_ITER_0)) 

#define FINALLY { exceptions_state_mc(CATCH_FINALLY); }
#define NO_FINALLY FINALLY
#define CATCH(exp)	%> if (exp.type != EXP_NO_EXCEPTION)
#define NO_CATCH(exp)	CATCH(exp) RETHROW(exp)

extern void
print_exception(struct exception_t * exp);

extern NORETURN ATTR_NORETURN void
throw_exception(enum exception_type type,
		uintptr_t val,
#ifdef SNITCHASER_DEBUG
		const char * file,
		const char * func,
		int line,
#endif
		const char * fmt, ...)
#ifdef SNITCHASER_DEBUG
ATTR(format(printf, 6, 7))
#else
ATTR(format(printf, 3, 4))
#endif
	;

#ifdef SNITCHASER_DEBUG
# define __dbg_info __FILE__, __FUNCTION__, __LINE__,
#else
# define __dbg_info
#endif

#define THROW(t, fmt...) throw_exception(t, 0, __dbg_info fmt)
#define THROW_VAL(t, v, fmt...) throw_exception(t, v, __dbg_info fmt)

#ifdef SNITCHASER_DEBUG
# define RETHROW(e) throw_exception((e).type, (e).u.xval, \
		(e).file, (e).func, (e).line, "%s", (char*)&((e).msg))
#else
# define RETHROW(e) throw_exception((e).type, (e).u.xval, \
		"%s", (char*)&((e).msg))
#endif


__END_DECLS
#endif
// vim:ts=4:sw=4

