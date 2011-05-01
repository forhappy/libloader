/* 
 * rebranch_tpd.h
 * by WN @ Oct. 28, 2010
 *
 * we want to decouple tls and tpd
 *
 */

#ifndef SNITCHASER_TPD_H
#define SNITCHASER_TPD_H

#include <linux/stddef.h>
#include <linux/kernel.h>

#include <loader/tls.h>
#include <loader/mm.h>

#include <loader/logger_tpd.h>
#include <loader/bigbuffer.h>
#include <loader/syscall_tpd.h>
#include <mutex.h>

struct thread_private_data {
	void * old_stack_top;
	void * stack_top;
	void * resume_addr;
	struct thread_private_data * tpd;
	pid_t pid;
	pid_t tid;
	struct bigbuffer bb;
	struct obj_mm_tpd_stub mm_stub;
	struct logger_tpd_stub logger_stub;
	struct syscall_tpd_stub syscall_stub;

	struct xmutex thread_mutex;
	sigset_t saved_sigset;
};

struct ___bottom_tls {
	struct thread_private_data tpd;
	struct tls_desc td;
};

#define tls_offset(e)	(TLS_AREA_SIZE - sizeof(struct ___bottom_tls) +\
		offsetof(struct ___bottom_tls, e))

#define tpd_offset(e)	(tls_offset(tpd.e))

static inline struct thread_private_data *
get_self_tpd(void)
{
	struct thread_private_data *tmp;
	asm volatile ("\
			movl %%fs:%c[offset], %[output]"
			:[output] "=r" (tmp)
			:[offset] "i" tpd_offset(tpd));
	return tmp;
}

#define def_get_addr_from_self_tpd(name, type, element)\
		static inline type*\
		get_self_##name##_ptr(void)\
		{\
			struct thread_private_data * tpd = get_self_tpd();\
			return &(tpd->element);\
		}

def_get_addr_from_self_tpd(logger, struct logger_tpd_stub, logger_stub)
def_get_addr_from_self_tpd(bb, struct bigbuffer, bb)
def_get_addr_from_self_tpd(syscall, struct syscall_tpd_stub, syscall_stub)

static inline struct thread_private_data *
get_tpd(struct tls_desc * td)
{
	void * topmost = get_tls_topmost_addr(td->start_addr);
	return topmost - sizeof(struct thread_private_data);
}

extern struct thread_private_data *
setup_self_tpd(void);

extern void
free_aux_mem(struct thread_private_data * tpd);

#endif

// vim:ts=4:sw=4

