SET(SRC
	log_syscalls.h
	log_syscalls.c
	handlers.h
	protos.h
	syscall_table.c
	syscall_table.h
	)

SET(SYSCALLS
	syscall_handler.h
	fstat64.c
	)

ADD_LIBRARY(log_syscall STATIC ${SRC})

ADD_LIBRARY(pre_library STATIC ${SYSCALLS})
ADD_LIBRARY(post_library STATIC ${SYSCALLS})
ADD_LIBRARY(replay_library STATIC ${SYSCALLS})


set_interp_properties(log_syscall)
set_interp_properties(pre_library)
set_interp_properties(post_library)
set_interp_properties(replay_library)

SET_PROPERTY(TARGET pre_library
	PROPERTY COMPILE_DEFINITIONS PRE_LIBRARY)
SET_PROPERTY(TARGET pre_library
	PROPERTY COMPILE_FLAGS ${KERNEL_CFLAGS})

SET_PROPERTY(TARGET post_library
	PROPERTY COMPILE_DEFINITIONS POST_LIBRARY)
SET_PROPERTY(TARGET post_library
	PROPERTY COMPILE_FLAGS ${KERNEL_CFLAGS})

SET_PROPERTY(TARGET replay_library
	PROPERTY COMPILE_DEFINITIONS REPLAY_LIBRARY)
SET_PROPERTY(TARGET replay_library
	PROPERTY COMPILE_FLAGS ${KERNEL_CFLAGS})



TARGET_LINK_LIBRARIES(log_syscall pre_library)
TARGET_LINK_LIBRARIES(log_syscall post_library)
TARGET_LINK_LIBRARIES(log_syscall replay_library)

# vim:ts=4:sw=4

