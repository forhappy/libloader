//#include <config.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <stdarg.h>
#include <errno.h>
#include <string.h>

#include "rb_debug.h"
#include "rb_exception.h"

static const char * exception_names[] = {
	[EXP_NO_EXCEPTION] = "no exception",
#define def_exp_type(a, b) \
		[a] = #a,
#include "rb_exception_types.h"
#undef def_exp_type
	[EXP_UNCATCHABLE] = "uncatchable exception",
};

static const char * exception_level_names[] DEBUG_DEF = {
	[EXP_LV_NONE] = "program is okay",
	[EXP_LV_LOWEST] = "program is safe",
	[EXP_LV_TAINTED] = "program is tainted",
	[EXP_LV_FATAL] = "program is unable to continue",
	[EXP_LV_UNCATCHABLE] = "program is dead",
};

/* 5 states of the catcher:
 *
 *                         +----------------(finally, cont)------------------------+
 *                         |                                                       V
 * INIT -(iter, cont)-> EXEC_BODY -(iter, stop)-> WAIT_FINALLY -(finally, cont)-> END
 *                         |                          ^
 *                    (throw, cont)                   |
 *                         |    ------(iter, stop)----+
 *                         V   /
 *                      THROWN
 * */

/* use a pointer to support potential multi-threading */
static struct catcher_t * curr_catcher_p = NULL;

static void
push_catcher(struct catcher_t * new_catcher)
{
	new_catcher->prev = curr_catcher_p;
	curr_catcher_p = new_catcher;
}

static void
pop_catcher(void)
{
	assert(curr_catcher_p != NULL);
	curr_catcher_p = curr_catcher_p->prev;
}

static void
init_set_exception(struct catcher_t * c, struct exception_t * e)
{
	memset((void*)e, '\0', sizeof(*e));
	memset(c, '\0', sizeof(*c));
	e->type = EXP_NO_EXCEPTION;
	e->level = EXP_LV_NONE;
	c->state = CATCHER_INIT;
	c->curr_exp = e;
}

EXCEPTIONS_SIGJMP_BUF *
exceptions_state_mc_init(struct catcher_t * c,
		volatile struct exception_t * e)
{
	init_set_exception(c, (struct exception_t *)e);
	push_catcher(c);
	return &(c->buf);
}

bool_t
exceptions_state_mc(enum catcher_action action)
{
	struct catcher_t * c = curr_catcher_p;
	assert(c != NULL);

#define MC_FAULT FATAL(SYSTEM, "exp mc: stat=%d(act=%d)\n", c->state, action); break;

	switch (c->state) {
	case CATCHER_INIT: {
		switch (action) {
		case CATCH_ITER_0:
			c->state = CATCHER_EXEC_BODY;
			return TRUE;
		default:
			MC_FAULT;
		}
	}
	case CATCHER_EXEC_BODY: {
		switch (action) {
		case CATCH_ITER_0:
			c->state = CATCHER_WAIT_FINALLY;
			return FALSE;
		case CATCH_THROW:
			c->state = CATCHER_THROWN;
			return FALSE;
		case CATCH_FINALLY:
			pop_catcher();
			return TRUE;
		default:
			MC_FAULT;
		}
	}
	case CATCHER_WAIT_FINALLY: {
		switch (action) {
		case CATCH_FINALLY: {
			pop_catcher();
			return TRUE;
		}
		default:
			MC_FAULT;
		}
	}
	case CATCHER_THROWN: {
		switch (action) {
		case CATCH_ITER_0: {
			c->state = CATCHER_WAIT_FINALLY;
			return FALSE;
		}
		default:
			MC_FAULT;
		}
	}
}
	FATAL(SYSTEM, "exception state machine in unknown state\n");
#undef MC_FAULT
	return FALSE;
}


void
print_exception(volatile struct exception_t * __exp)
{
	struct exception_t * exp = (struct exception_t *)__exp;
#ifdef REBRANCH_DEBUG
	/* enum us unsigned, so >= 0 */
	assert(exp->level < NR_EXP_LEVELS);
	assert(exp->type < NR_EXP_TYPES);
	WARNING(SYSTEM, "(%s): %s raised at file %s [%s:%d]:\n",
			exception_level_names[exp->level],
			exception_names[exp->type],
			exp->file, exp->func, exp->line);
#else
	WARNING(SYSTEM, "%s raised:\n", exception_names[exp->type]);
#endif
	WARNING(SYSTEM, "\tmessage: %s\n", exp->msg);
	WARNING(SYSTEM, "\twith value: %d(%p)\n", exp->u.val, exp->u.ptr);
	WARNING(SYSTEM, "\twith errno: %d(%s)\n", exp->throw_time_errno,
			strerror(exp->throw_time_errno));
	errno = 0;
}

NORETURN ATTR_NORETURN 
static void
__throw_exception(enum exception_type type,
		uintptr_t val,
		enum exception_level level,
#ifdef REBRANCH_DEBUG
		const char * file,
		const char * func,
		int line,
#endif
		const char * fmt, va_list ap)
{
	struct exception_t exp;

	exp.type = type;
	exp.throw_time_errno = errno;

	vsnprintf(exp.msg, sizeof(exp.msg), fmt, ap);

	exp.u.ptr = (void*)val;
	exp.type = type;
	exp.level = level;
	if (level >= EXP_LV_UNCATCHABLE) {
		exp.type = EXP_UNCATCHABLE;
		exp.level = EXP_LV_UNCATCHABLE;
	}
#ifdef REBRANCH_DEBUG
	exp.file = file;
	exp.func = func;
	exp.line = line;
#endif
	if (curr_catcher_p == NULL) {
		print_exception(&exp);
		FATAL(SYSTEM, "throw exception from outmost scope\n");
	}

	*(curr_catcher_p->curr_exp) = exp;

	exceptions_state_mc(CATCH_THROW);
	/* do the jump! */
	EXCEPTIONS_SIGLONGJMP(curr_catcher_p->buf, type);
}

NORETURN ATTR_NORETURN 
#ifdef REBRANCH_DEBUG
ATTR(format(printf, 7, 8))
#else
ATTR(format(printf, 4, 5))
#endif
void
throw_exception(enum exception_type type,
		uintptr_t val,
		enum exception_level level,
#ifdef REBRANCH_DEBUG
		const char * file,
		const char * func,
		int line,
#endif
		const char * fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
#ifdef REBRANCH_DEBUG
	__throw_exception(type, val, level,
			file, func, line, fmt, ap);
#else
	__throw_exception(type, val, level,
			fmt, ap);
#endif
	va_end(ap);
}


// vim:ts=4:sw=4:cino=l1,\:0
