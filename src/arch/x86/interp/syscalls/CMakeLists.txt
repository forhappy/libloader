SET(SRC
	log_syscalls.h
	log_syscalls.c
	)

ADD_LIBRARY(log_syscall STATIC ${SRC})

set_interp_properties(log_syscall)

# vim:ts=4:sw=4
