/* 
 * syscalls.c
 * by WN @ Oct. 14, 2010
 *
 * implement syscall.h
 */

#include "asm_syscall.h"
#include "syscall.h"
#include <linux/net.h>

ssize_t
sys_write(int fd, const void * buffer, size_t size)
{
	ssize_t res;
	res = SYSCALL_MACRO(write, 3,
			fd, buffer, size);
	return res;
}

ATTR(noreturn) void
sys_exit(int val)
{
	SYSCALL_MACRO(exit, 1, val);
	while(1);
}

ATTR(noreturn) void
sys_exit_group(int val)
{
	SYSCALL_MACRO(exit_group, 1, val);
	while(1);
}


int
sys_close(int fd)
{
	return SYSCALL_MACRO(close, 1, fd);
}

int
sys_dup2(int fd1, int fd2)
{
	return SYSCALL_MACRO(dup2, 2, fd1, fd2);
}

int
sys_personality(unsigned long int persona)
{
	return SYSCALL_MACRO(personality, 1, persona);
}

int
sys_execve(const char * filename, const char ** args, const char ** envs)
{
	return SYSCALL_MACRO(execve, 3, filename, args, envs);
}

int
sys_open(const char * filename, int flags, int mode)
{
	return SYSCALL_MACRO(open, 3, filename, flags, mode);
}

ssize_t
sys_read(int fd, void * buffer, size_t sz)
{
	return SYSCALL_MACRO(read, 3, fd, buffer, sz);
}

int
sys_lseek(int fd, off_t offset, unsigned int origin)
{
	return SYSCALL_MACRO(lseek, 3, fd, offset, origin);
}

void *
sys_mmap2(void * addr, size_t length, int prot, int flags,
		int fd, off_t pgoffset)
{
	return (void*)SYSCALL_MACRO(mmap2, 6,
			addr, length, prot, flags, fd, pgoffset);
}

void *
sys_mremap(void * addr,size_t old_len, size_t new_len,
			   int flags, void * new_addr)
{
	return (void *)SYSCALL_MACRO(mremap, 5, 
			addr, old_len, new_len, flags, new_addr);
}

int
sys_munmap(void * addr, size_t length)
{
	return SYSCALL_MACRO(munmap, 2, addr, length);
}

int
sys_getpid(void)
{
	return SYSCALL_MACRO(getpid, 0);
}

int
sys_gettid(void)
{
	return SYSCALL_MACRO(gettid, 0);
}

int
sys_mprotect(void * ptr, size_t sz, uint32_t prot)
{
	return SYSCALL_MACRO(mprotect, 3,
			ptr, sz, prot);
}

int
sys_wait4(pid_t pid, int * stat_addr, int options, struct rusage * ru)
{
	return SYSCALL_MACRO(wait4, 4, pid, stat_addr, options, ru);
}


int
sys_gettimeofday(struct timeval * tv, struct timezone * tz)
{
	return SYSCALL_MACRO(gettimeofday, 2, tv, tz);
}


uintptr_t
sys_brk(void * addr)
{
	return SYSCALL_MACRO(brk, 1, addr);
}



int
sys_stat64(const char * fn, struct stat64 * st)
{
	return SYSCALL_MACRO(stat64, 2, fn, st);
}

int
sys_get_thread_area(struct user_desc * u_info)
{
	return SYSCALL_MACRO(get_thread_area, 1, u_info);
}

int
sys_set_thread_area(struct user_desc * u_info)
{
	return SYSCALL_MACRO(set_thread_area, 1, u_info);
}


/*
int
sys_rt_sigprocmask(int how, sigset_t * set, sigset_t * oset,
		size_t sigsetsize)
{
	return SYSCALL_MACRO(rt_sigprocmask, 4,
			how, set, oset, sigsetsize);
}
*/


int
sys_fstat64(unsigned long fd, struct stat64 * statbuf)
{
	return SYSCALL_MACRO(fstat64, 2, fd, statbuf);
}


pid_t
sys_fork(void)
{
	return SYSCALL_MACRO(fork, 0);
}


/*
int
sys_rt_sigaction(int sig, const struct sigaction * act,
		struct sigaction * oact, size_t sigsetsize)
{
	return SYSCALL_MACRO(rt_sigaction, 4,
			sig, act, oact, sigsetsize);
}
*/


int
sys_tgkill(int tgid, int pid, int sig)
{
	return SYSCALL_MACRO(tgkill, 3,
			tgid, pid, sig);
}

int
sys_kill(int pid, int sig)
{
	return SYSCALL_MACRO(kill, 2,
			pid, sig);
}


/*
int
sys_select(int n, fd_set * inp, fd_set * outp, fd_set * exp,
		struct timeval * tvp)
{
	return SYSCALL_MACRO(_newselect, 5, n, inp, outp, exp, tvp);
}
*/


extern int
sys_send(int fd, const void * buf, size_t len, unsigned flags)
{
	unsigned long args[4];
	args[0] = (unsigned long)(fd);
	args[1] = (unsigned long)(buf);
	args[2] = (unsigned long)(len);
	args[3] = (unsigned long)(flags);
	return SYSCALL_MACRO(socketcall, 2, SYS_SEND, args);
}

int
sys_recv(int fd, void * buf, size_t len, unsigned flags)
{
	unsigned long args[4];
	args[0] = (unsigned long)(fd);
	args[1] = (unsigned long)(buf);
	args[2] = (unsigned long)(len);
	args[3] = (unsigned long)(flags);
	return SYSCALL_MACRO(socketcall, 2, SYS_RECV, args);
}

// vim:ts=4:sw=4

