INCLUDE_DIRECTORIES(
	${PROJECT_BINARY_DIR}
	${PROJECT_SOURCE_DIR}/src/include
	${PROJECT_SOURCE_DIR}/src/core/
	)

SET(SOURCE
	brk.c
	close.c
	fstat64.c
	futex.c
	getpid.c
	gettimeofday.c
	ioctl.c
	mmap.c
	mmap2.c
	mprotect.c
	munmap.c
	nanosleep.c
	open.c
	read.c
	rt_sigaction.c
	rt_sigprocmask.c
	set_robust_list.c
	set_thread_area.c
	set_tid_address.c
	stat64.c
	statfs64.c
	syscall_tbl.c
	ugetrlimit.c
	uname.c
	write.c
	)
ADD_LIBRARY(log_syscall ${SOURCE})
SET_PROPERTY(TARGET log_syscall 
	PROPERTY COMPILE_FLAGS "${LOADER_COMPILE_FLAGS}")
SET_PROPERTY(TARGET log_syscall
	PROPERTY LINK_FLAGS "${LOADER_LINK_FLAGS}")
