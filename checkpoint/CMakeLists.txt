
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
	nanosleep.c
	ioctl.c
	ioctl_tty.c
	fcntl64.c
	getdents64.c
	)

ADD_LIBRARY(checkpoint
	checkpoint.c
	checkpoint.h
	syscall_table.c
	syscall_table.h
	${HANDLERS}
	)

ADD_LIBRARY(printer
	checkpoint.h
	syscall_table.c
	syscall_table.h
	${HANDLERS}
	)

SET_TARGET_PROPERTIES(
	printer
	PROPERTIES COMPILE_FLAGS -DSYSCALL_PRINTER
	)

