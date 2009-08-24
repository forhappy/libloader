all: libcheckpoint.os

VPATH = ../../checkpoint

SYSCALLS = simple.c close.c mprotect.c read.c set_robust_list.c sigprocmask.c ugetrlimit.c brk.c fstat64.c \
	   munmap.c rt_sigaction.c set_thread_area.c stat64.c uname.c mmap2.c open.c rt_sigprocmask.c \
	   set_tid_address.c write.c nanosleep.c ioctl.c ioctl_tty.c fcntl64.c getdents64.c exit_group.c \
	   time.c socketcall.c socket.c bind.c getsockname.c sendto.c recvfrom.c recvmsg.c connect.c _newselect.c \
	   recv.c _llseek.c clock_getres.c lstat64.c getpeername.c futex.c clock_gettime.c \
	   getcwd.c setsockopt.c listen.c poll.c accept.c \
	   writev.c sendfile64.c shutdown.c getrusage.c gettimeofday.c pipe.c clone.c newprocess.c kill.c \
	   waitpid.c send.c dup.c dup2.c wait4.c socketpair.c ioctl_blk.c sendmsg.c setitimer.c epoll_wait.c \
	   times.c

CHECKPOINT_SRCS = $(SYSCALLS)
CHECKPOINT_HEADERS = checkpoint.h checkpoint.c syscalls.h syscall_table.h

CHECKPOINT_SRCS = $(SYSCALLS) $(CHECKPOINT_HEADERS) syscall_table.c

CFLAGS= -g -Wall -std=gnu99 -nostartfiles -nodefaultlibs -nostdlib -fno-builtin -fvisibility=hidden -fPIC \
	-DIN_INJECTOR -I../.. -I../../checkpoint -I../

libcheckpoint.os: $(patsubst %.c,%.os, $(SYSCALLS) checkpoint.c syscall_table.c)
	ld -r $^ -o $@

%.os: %.c
	gcc -fno-builtin $(CFLAGS) -c $^ -o $@

.PHONY: clean

clean:
	rm -f *.o *.os *.so

#$(CHECKPOINT_SRCS):
#	@if ! test -e $@ ; then ln -s ../../checkpoint/$@ .; fi

