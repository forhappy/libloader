/*
 * logger.c
 * by WN @ Nov. 12, 2010
 */

#include <common/defs.h>
#include <common/debug.h>
#include <interp/bigbuffer.h>
#include <interp/logger.h>
#include <interp/compression.h>
#include <xasm/syscall.h>
#include <linux/kernel.h>
#include <linux/fcntl.h>
#include <xasm/mutex.h>
#include <interp/tls.h>
#include <interp/rebranch_asm_offset.h>
#include <interp/checkpoint.h>

#define ASM_BASE	(tpd_offset(bb.buffer[0]))

inline static struct bigbuffer *
get_this_bb(struct logger_tpd_stub * l)
{
    struct thread_private_data * tpd =
        container_of(l, struct thread_private_data, logger_stub);
    return &(tpd->bb);
}

void
init_logger(struct logger_tpd_stub * l, struct timeval * ptv, int pid, int tid)
{
    DEBUG(LOGGER, "logger for %d-%d initing\n", pid, tid);
    assert(l != NULL);

    struct bigbuffer * pbb = get_this_bb(l);

    l->this_bigbuffer = pbb;
    reset_bigbuffer_ptr(pbb);

    size_t sz_fn = snprintf(l->log_fn, MAX_OUTPUT_FN_LEN - 1,
                            "%s/%d-%d-%010u-%010u.log", LOG_DIR, pid, tid,
                            (uint32_t)ptv->tv_sec, (uint32_t)ptv->tv_usec);
    assert(sz_fn < MAX_OUTPUT_FN_LEN);
    TRACE(LOGGER, "log file name is set to %s\n", l->log_fn);

    l->start_ptr = &(pbb->buffer[pbb->ptr]);
    l->end_ptr = &(pbb->buffer[BIGBUFFER_SZ]);
#if BIGBUFFER_SZ <= 32
# error bigbuffer too small
#endif
    l->asm_curr_ptr = ASM_BASE;
    l->asm_end_ptr = l->asm_curr_ptr + BIGBUFFER_SZ - 32;
    DEBUG(LOGGER, "asm_curr_ptr set to 0x%lx\n", l->asm_curr_ptr);
    DEBUG(LOGGER, "asm_end_ptr set to 0x%lx\n", l->asm_end_ptr);


    /* init the flush mutex */
    xmutex_init(&l->log_flush_mutex);

    /* write comp file header */
    int fd = sys_open(l->log_fn, O_WRONLY | O_CREAT | O_TRUNC, 0664);
    if (fd < 0) {
        WARNING(LOGGER, "open logger file %s failed: %d\n",
                l->log_fn, fd);
        return;
    }
    write_comp_file_head(fd);
    sys_close(fd);
}

void
switch_logger(struct logger_tpd_stub * l, struct timeval * ptv, int pid, int tid)
{
    DEBUG(LOGGER, "logger for %d-%d switching\n", pid, tid);
    assert(l != NULL);

    struct bigbuffer * pbb = get_this_bb(l);

    l->this_bigbuffer = pbb;
    reset_bigbuffer_ptr(pbb);

    size_t sz_fn = snprintf(l->log_fn, MAX_OUTPUT_FN_LEN - 1,
                            "%s/%d-%d-%010u-%010u.log", LOG_DIR, pid, tid,
                            (uint32_t)ptv->tv_sec, (uint32_t)ptv->tv_usec);
    assert(sz_fn < MAX_OUTPUT_FN_LEN);
    TRACE(LOGGER, "log file name is set to %s\n", l->log_fn);

    l->start_ptr = &(pbb->buffer[pbb->ptr]);
    l->end_ptr = &(pbb->buffer[BIGBUFFER_SZ]);
#if BIGBUFFER_SZ <= 32
# error bigbuffer too small
#endif
    l->asm_curr_ptr = ASM_BASE;
    l->asm_end_ptr = l->asm_curr_ptr + BIGBUFFER_SZ - 32;
    DEBUG(LOGGER, "asm_curr_ptr set to 0x%lx\n", l->asm_curr_ptr);
    DEBUG(LOGGER, "asm_end_ptr set to 0x%lx\n", l->asm_end_ptr);


    /* init the flush mutex */
    xmutex_init(&l->log_flush_mutex);

    /* write comp file header */
    int fd = sys_open(l->log_fn, O_WRONLY | O_CREAT | O_TRUNC, 0664);
    if (fd < 0) {
        WARNING(LOGGER, "open logger file %s failed: %d\n",
                l->log_fn, fd);
        return;
    }
    write_comp_file_head(fd);
    sys_close(fd);
}

void
append_logger(void * data, size_t size)
{
    struct logger_tpd_stub * l = get_self_logger_ptr();
    while (size > 0) {
        size_t aval = l->asm_end_ptr - l->asm_curr_ptr;
        size_t cpysz = aval > size ? size : aval;
        memcpy(l->start_ptr + (l->asm_curr_ptr - ASM_BASE),
               data, cpysz);
        data += cpysz;
        size -= cpysz;
        l->asm_curr_ptr += cpysz;
        if (size > 0) {
            flush_logger(l);
        }
    }
}

void
flush_logger(struct logger_tpd_stub * l)
{
    assert(l != NULL);

    sigset_t orimask = xmutex_lock(&l->log_flush_mutex);

    struct bigbuffer * pbb = get_this_bb(l);

    int err;

    uintptr_t curr_offset = l->asm_curr_ptr - ASM_BASE;


    set_bigbuffer_ptr(pbb, curr_offset);
    assert(curr_offset < sizeof(pbb->buffer));

    int fd = sys_open(l->log_fn, O_WRONLY|O_APPEND|O_CREAT, 0664);
    struct stat64 statbuf;
    err = sys_fstat64(fd, &statbuf);
    if (statbuf.st_size > 1024 * 1024 ) {
        TRACE(LOGGER, "coming to the logger\n");
        struct pusha_regs regs;
        /*
         *
         * struct pusha_regs {
         *		uint32_t eflags;
         *		uint32_t edi;
         *		uint32_t esi;
         *		uint32_t ebp;
         *		uint32_t esp;
         *		uint32_t ebx;
         *		uint32_t edx;
         *		uint32_t ecx;
         *		uint32_t eax;
         * };
         * */
#if 0
        asm volatile ("\
				pusha\n\
				pushf\n\
				movl $0, %edi\n\
				popl regs(, %edi, 4)\n\
				incl %edi\n\
				popl regs(, %edi, 4)\n\
				incl %edi\n\
				popl regs(, %edi, 4)\n\
				incl %edi\n\
				popl regs(, %edi, 4)\n\
				incl %edi\n\
				popl regs(, %edi, 4)\n\
				incl %edi\n\
				popl regs(, %edi, 4)\n\
				incl %edi\n\
				popl regs(, %edi, 4)\n\
				incl %edi\n\
				popl regs(, %edi, 4)\n\
				incl %edi\n\
				popl regs(, %edi, 4)\n\
				");
#endif
        TRACE(LOGGER, "logger size %lld\n", statbuf.st_size);
#if 1
        struct thread_private_data * tpd = get_self_tpd();
        struct timeval tv;
        int err = sys_gettimeofday(&tv, NULL);
        assert(err == 0);

        tpd->pid = sys_getpid();
        tpd->tid = sys_gettid();

        switch_logger(l, &tv, tpd->pid, tpd->tid);

        int newfd = sys_open(l->log_fn, O_RDWR | O_APPEND | O_CREAT, 0664);
        TRACE(LOGGER, "fd %d, newfd %d new file name %s\n", fd, newfd, l->log_fn);
        //sys_dup2(newfd, fd);

#if 0
        err = sys_dup2(newfd, fd);
        if (err == -1) {
            xmutex_unlock(&l->log_flush_mutex, orimask);
            FATAL(LOGGER, "sys_dup2 oldfd %d,newfd %d, error: %d\n", newfd, fd, err);
        }
        TRACE(LOGGER, "fd %d\n", fd);
#endif
        if (newfd < 0) {
            xmutex_unlock(&l->log_flush_mutex, orimask);
            WARNING(LOGGER, "open log file %s failed: %d\n", l->log_fn, newfd);
            reset_bigbuffer_ptr(pbb);
            l->asm_curr_ptr = ASM_BASE;
            return;
        }

        /* flush bigbuffer */
        flush_bigbuffer(newfd, pbb);
        reset_bigbuffer_ptr(pbb);

        err = sys_close(fd);
        if (err != 0) {
            xmutex_unlock(&l->log_flush_mutex, orimask);
            FATAL(LOGGER, "close log file %d error: %d\n", fd, err);
        }
        err = sys_close(newfd);
        if (err != 0) {
            xmutex_unlock(&l->log_flush_mutex, orimask);
            FATAL(LOGGER, "close log file %d error: %d\n", newfd, err);
        }

        /* reset asm_curr_ptr */
        l->asm_curr_ptr = ASM_BASE;
        xmutex_unlock(&l->log_flush_mutex, orimask);
        return;
#endif
    }
    if (fd < 0) {
        xmutex_unlock(&l->log_flush_mutex, orimask);
        WARNING(LOGGER, "open log file %s failed: %d\n", l->log_fn, fd);
        reset_bigbuffer_ptr(pbb);
        l->asm_curr_ptr = ASM_BASE;
        return;
    }

    /* flush bigbuffer */
    flush_bigbuffer(fd, pbb);
    reset_bigbuffer_ptr(pbb);

    err = sys_close(fd);
    if (err != 0) {
        xmutex_unlock(&l->log_flush_mutex, orimask);
        FATAL(LOGGER, "close log file %d error: %d\n", fd, err);
    }

    /* reset asm_curr_ptr */
    l->asm_curr_ptr = ASM_BASE;
    xmutex_unlock(&l->log_flush_mutex, orimask);
}


void
flush_all_logger(void)
{
    sigset_t oldmask = xmutex_lock(&tls_lock);

    struct tls_desc * td;
    list_for_each_entry(td, &tls_list, list) {
        struct logger_tpd_stub * log =
            (struct logger_tpd_stub *)
            (td->start_addr + OFFSET_TPD_LOGGER_STUB);
        flush_logger(log);
    }

    xmutex_unlock(&tls_lock, oldmask);
}


// vim:ts=4:sw=4

