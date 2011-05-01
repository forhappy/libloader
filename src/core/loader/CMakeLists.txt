INCLUDE_DIRECTORIES(
	${PROJECT_BINARY_DIR}
	${PROJECT_SOURCE_DIR}/src/include
	${PROJECT_SOURCE_DIR}/src/core/
	)

SET(SOURCE
	debug.c
	fdbnprintf.c
	startup_stack.c
	env_conf.c
	snitchaser_main.c
	elf.c
	syscall.c
	wrapper.c
	processor.c
	proc.c
	interp_main.c
	tls.c
	mm.c
	mutex.c
	bigbuffer.c
	)
ADD_LIBRARY(loader SHARED ${SOURCE})

SET_PROPERTY(TARGET loader 
	PROPERTY COMPILE_FLAGS "${LOADER_COMPILE_FLAGS}")
SET_PROPERTY(TARGET loader
	PROPERTY LINK_FLAGS "${LOADER_LINK_FLAGS}")
ADD_DEFINITIONS(-DINTERP_START_ENTRY)
TARGET_LINK_LIBRARIES(loader xprintf dummy log_syscall)
