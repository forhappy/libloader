
SET(HANDLERS
	brk.c
	uname.c
	mmap2.c
	access.c
	open.c
	fstat64.c
	close.c
	stat64.c
	read.c
	set_thread_area.c
	mprotect.c
	munmap.c
	set_tid_address.c
	set_robust_list.c
	rt_sigaction.c
	rt_sigprocmask.c
	ugetrlimit.c
	write.c
	)

ADD_LIBRARY(checkpoint
	checkpoint.c
	checkpoint.h
	syscall_table.c
	syscall_table.h
	${HANDLERS}
	)



