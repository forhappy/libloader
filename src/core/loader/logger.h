/* 
 * logger.h
 * by WN @ Nov. 12, 2010
 * */

#ifndef __LOGGER_H
#define __LOGGER_H

#include <config.h>
#include <loader/logger_tpd.h>
#include <loader/snitchaser_tpd.h>
#include <loader/tls.h>

#ifdef __KERNEL__
# include <linux/time.h>
#else
# include <sys/time.h>
#endif

extern void
init_logger(struct logger_tpd_stub * l, struct timeval * ptv, int pid, int tid);

extern void
flush_logger(struct logger_tpd_stub * l);

static inline void
init_self_logger(struct timeval * ptv, int pid, int tid)
{
	struct logger_tpd_stub * l = get_self_logger_ptr();
	init_logger(l, ptv, pid, tid);
}

static inline void
flush_self_logger(void)
{
	struct logger_tpd_stub * l = get_self_logger_ptr();
	flush_logger(l);
}

extern void
flush_all_logger(void);

extern void
append_logger(void * data, size_t size);

#define def_append_logger_TYPE(t, tn)\
	inline static void\
	append_logger_##tn(t x)\
	{\
		struct logger_tpd_stub * l = get_self_logger_ptr();\
		if (l->asm_curr_ptr + sizeof(x) > l->asm_end_ptr) {\
			flush_logger(l);\
		}\
		struct tls_desc * td = get_self_tls_desc();\
		t * ptr = td->start_addr + l->asm_curr_ptr;\
		*ptr = x;\
		l->asm_curr_ptr += sizeof(t);\
	}

def_append_logger_TYPE(uint32_t, u32)
def_append_logger_TYPE(void *, ptr)
def_append_logger_TYPE(uint16_t, u16)
def_append_logger_TYPE(uint8_t, u8)
def_append_logger_TYPE(int, int)

#undef def_append_logger_TYPE

#endif

// vim:ts=4:sw=4

