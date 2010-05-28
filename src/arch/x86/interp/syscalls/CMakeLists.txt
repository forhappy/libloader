SET(SRC
	log_syscalls.h
	log_syscalls.c
	handlers.h
	protos.h
	syscall_table.c
	syscall_table.h
	)

ADD_LIBRARY(log_syscall STATIC ${SRC})

set_interp_properties(log_syscall)

# vim:ts=4:sw=4
