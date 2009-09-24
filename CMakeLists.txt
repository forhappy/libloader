CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

PROJECT(SNITCHASER)

IF (NOT ${SNITCHASER_BINARY_DIR} STREQUAL ${SNITCHASER_SOURCE_DIR})
	MESSAGE(FATAL_ERROR "currently only In-tree-compile is supported")
ENDIF (NOT ${SNITCHASER_BINARY_DIR} STREQUAL ${SNITCHASER_SOURCE_DIR})

# use -O2 can reveal some error, like uninitialized vars
SET(CMAKE_C_FLAGS "-g -std=gnu99 -O2 -Wall -D_GNU_SOURCE -D_FILE_OFFSET_BITS=64")
SET(CMAKE_C_FLAGS_DEBUG "-g -O0")
SET(CMAKE_C_FLAGS_RELEASE "-O2 -DNDEBUG")

INCLUDE_DIRECTORIES(${SNITCHASER_BINARY_DIR}
	${SNITCHASER_SOURCE_DIR})

SET(COMMON
	defs.h
	debug.c
	debug.h
	exception.c
	exception.h
	utils.c
	utils.h)

SET(PTRACE
	ptraceutils.h
	ptraceutils.c
	)

SET(ELF
	elfutils.c
	elfutils.h
	)

SET(PROC
	procutils.h
	procutils.c
	)
SET(UTILS
	utils.c
	utils.h
	)

ADD_LIBRARY(common
	${COMMON}
	${PTRACE}
	${PROC}
	${ELF}
	ckptutils.c
	)

ADD_EXECUTABLE(target
	target.c
)

ADD_EXECUTABLE(testerrno
	testerrno.c)


ADD_EXECUTABLE(testmath
	testmath.c)

ADD_EXECUTABLE(testsignal
	testsignal.c)

SET_TARGET_PROPERTIES(target testerrno testmath
	PROPERTIES COMPILE_FLAGS "-g")

TARGET_LINK_LIBRARIES(testerrno "-lpthread")

ADD_SUBDIRECTORY(checkpoint)

ADD_EXECUTABLE(snitchaser
	snitchaser.c
	snitchaser_args.c
	libwrapper.so
	)

ADD_EXECUTABLE(printlog
	printlog.c
	)

ADD_EXECUTABLE(gdbloader
	gdbloader.c
	gdbloader_args.c
	)

TARGET_LINK_LIBRARIES(gdbloader
	common)

TARGET_LINK_LIBRARIES(snitchaser
	checkpoint
	common)

TARGET_LINK_LIBRARIES(printlog
	common
	printer)

SET_TARGET_PROPERTIES(
	printlog
	PROPERTIES COMPILE_FLAGS -DSYSCALL_PRINTER
	)



SET(RELAX_SIGNAL OFF
	CACHE BOOL "disable signal when trapped")

SET(FORK_CKPT ON
	CACHE BOOL "make ckpt by a forked process")

SET(RELAX_SIGNAL_OPT "")
IF (RELAX_SIGNAL)
	SET(RELAX_SIGNAL_OPT "RELAX_SIGNAL=-DRELAX_SIGNAL")
ENDIF (RELAX_SIGNAL)

SET(FORK_CKPT_OPT "")
IF (FORK_CKPT)
	SET(FORK_CKPT_OPT "FORK_CKPT=-DFORK_CKPT")
ENDIF (FORK_CKPT)


ADD_CUSTOM_COMMAND(OUTPUT libwrapper.so
	COMMAND make -C libwrapper ${RELAX_SIGNAL_OPT} ${FORK_CKPT_OPT}
	WORKING_DIRECTORY ${SNITCHASER_SOURCE_DIR}
	VERBATIM
	)


INCLUDE(CMakeDependentOption)
INCLUDE(CheckIncludeFiles)
INCLUDE(CheckFunctionExists)
INCLUDE(CheckSymbolExists)

SET(LOGGER_DIRECTORY "/tmp"
	CACHE STRING "logger directory")

CHECK_INCLUDE_FILES(malloc.h HAVE_MALLOC_H)
CHECK_INCLUDE_FILES(alloca.h HAVE_ALLOCA_H)
IF (HAVE_ALLOCA_H)
	SET(HAVE_ALLOCA TRUE)
ENDIF (HAVE_ALLOCA_H)
CHECK_FUNCTION_EXISTS(atexit HAVE_ATEXIT)
CHECK_FUNCTION_EXISTS(sigaction HAVE_SIGACTION)
CHECK_INCLUDE_FILES(execinfo.h HAVE_EXECINFO_H)
CHECK_FUNCTION_EXISTS(backtrace HAVE_BACKTRACE)
CHECK_FUNCTION_EXISTS(mallinfo HAVE_MALLINFO)
CHECK_FUNCTION_EXISTS(malloc_stats HAVE_MALLOC_STATS)

CHECK_INCLUDE_FILES(stdbool.h HAVE_STDBOOL_H)
CHECK_INCLUDE_FILES(setjmp.h HAVE_SETJMP_H)
CHECK_SYMBOL_EXISTS(sigsetjmp setjmp.h HAVE_SIGSETJMP)
CHECK_FUNCTION_EXISTS(clock_gettime HAVE_CLOCK_GETTIME)
CONFIGURE_FILE(config.h.cmake.in config.h)


