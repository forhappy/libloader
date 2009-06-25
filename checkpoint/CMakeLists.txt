
SET(HANDLERS
	brk.c
	uname.c
	mmap2.c
	access.c
	)

ADD_LIBRARY(checkpoint
	checkpoint.c
	checkpoint.h
	syscall_table.c
	syscall_table.h
	${HANDLERS}
	)



