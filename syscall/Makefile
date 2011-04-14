all:liblogckpt.os
CFLAGS += -g -std=gnu99 -Wall -DINTERP_START_ENTRY -DELF32CLASS \
		  -nostartfiles -nodefaultlibs -nostdlib -fno-builtin -fvisibility=hidden -fPIC 

liblogckpt.os:syscall_tbl.o brk.o fstat64.o gettimeofday.o ioctl.o mmap.o mmap2.o mprotect.o munmap.o open.o read.o \
	stat64.o write.o close.o getpid.o rt_sigprocmask.o rt_sigaction.o nanosleep.o set_tid_address.o \
	set_thread_area.o set_robust_list.o futex.o ugetrlimit.o uname.o
	ld -r $^ -o $@
	cp liblogckpt.os ../liblogckpt.os

syscall_tbl.o:syscall_tbl.c syscall_tbl.h
brk.o:brk.c
fstat64.o:fstat64.c
gettimeofday.o:gettimeofday.c
ioctl.o:ioctl.c
mmap.o:mmap.c
mmap2.o:mmap2.c
mprotect.o:mprotect.c
munmap.o:munmap.c
open.o:open.c
read.o:read.c
stat64.o:stat64.c
write.o:write.c
close.o:close.c
getpid.o:getpid.c
rt_sigprocmask.o:rt_sigprocmask.c
rt_sigaction.o:rt_sigaction.c
nanosleep.o:nanosleep.c
set_tid_address.:set_tid_address.c
set_thread_area.o:set_thread_area.c
set_robust_list.o:set_robust_list.c
futex.o:futex.c
ugetrlimit.o:ugetrlimit.c
uname.o:uname.c

.PHONY:clean
clean:
	rm *.o *.os 
