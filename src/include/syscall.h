/* 
 * syscalls.h
 * by WN @ Oct. 14, 2010
 * modified by HP.Fu @ Apr. 4, 2011
 */

#ifndef __SYSCALL_H
#define __SYSCALL_H
#include <defs.h>
#include <linux/resource.h>
#include <linux/types.h>
#include <linux/time.h>
#include <linux/stat.h>
#include <asm/ldt.h>
#include <linux/signal.h>

extern ssize_t
sys_write(int fd, const void * buffer, size_t size);

extern void ATTR(noreturn)
sys_exit(int val);

extern void ATTR(noreturn)
sys_exit_group(int val);

extern int
sys_close(int fd);

extern int
sys_dup2(int fd1, int fd2);

extern int
sys_personality(unsigned long int persona);

extern int
sys_execve(const char * filename, const char ** args, const char ** envs);

extern int
sys_open(const char * filename, int flags, int mode);

extern ssize_t
sys_read(int fd, void * buffer, size_t sz);

extern int
sys_lseek(int fd, off_t offset, unsigned int origin);

extern void *
sys_mmap2(void * addr, size_t length, int prot, int flags,
		int fd, off_t pgoffset);

extern void *
sys_mremap(void * addr,size_t old_len, size_t new_len,
			   int flags, void * new_addr);

static inline void *
sys_uniform_mmap(void * addr, size_t length, int prot, int flags,
		int fd, uint64_t offset)
{
	return sys_mmap2(addr, length, prot, flags, fd, offset >> 12);
}

extern int
sys_munmap(void * addr, size_t length);

extern int
sys_getpid(void);

extern int
sys_gettid(void);

extern int
sys_mprotect(void * ptr, size_t sz, uint32_t prot);

extern int
sys_wait4(pid_t pid, int * stat_addr, int options, struct rusage * ru);


extern int
sys_gettimeofday(struct timeval * tv, struct timezone * tz);

extern uintptr_t
sys_brk(void * addr);

extern int
sys_stat64(const char * fn, struct stat64 * st);

extern int
sys_get_thread_area(struct user_desc * u_info);

extern int
sys_set_thread_area(struct user_desc * u_info);
/*
extern int
sys_rt_sigprocmask(int how, sigset_t * set, sigset_t * oset,
		size_t sigsetsize);
*/

extern int
sys_fstat64(unsigned long fd, struct stat64 * statbuf);

extern pid_t
sys_fork(void);
/*
extern int
sys_rt_sigaction(int sig, const struct sigaction * act,
		struct sigaction * oact, size_t sigsetsize);
*/
extern int
sys_tgkill(int tgid, int pid, int sig);

extern int
sys_kill(int pid, int sig);

/* use sysnr 142, named _newselect */
/*
extern int
sys_select(int n, fd_set * inp, fd_set * outp, fd_set * exp,
		struct timeval * tvp);
*/
extern int
sys_send(int fd, const void * buf, size_t len, unsigned flags);

extern int
sys_recv(int fd, void * buf, size_t len, unsigned flags);
#endif

// vim:ts=4:sw=4

