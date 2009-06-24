CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

PROJECT(CURRF2)

IF (NOT ${CURRF2_BINARY_DIR} STREQUAL ${CURRF2_SOURCE_DIR})
	MESSAGE(FATAL_ERROR "currently only In-tree-compile is supported")
ENDIF (NOT ${CURRF2_BINARY_DIR} STREQUAL ${CURRF2_SOURCE_DIR})

SET(CMAKE_C_FLAGS "-std=gnu99 -Wall -D_GNU_SOURCE -D_FILE_OFFSET_BITS=64")
SET(CMAKE_C_FLAGS_DEBUG "-g -O0")
SET(CMAKE_C_FLAGS_RELEASE "-O2 -DNDEBUG")

INCLUDE_DIRECTORIES(${CURRF2_BINARY_DIR}
	${CURRF2_SOURCE_DIR})

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

ADD_EXECUTABLE(target
	target.c
)

#ADD_EXECUTABLE(test_utils
#	${COMMON}
#	)
#SET_TARGET_PROPERTIES(test_utils
#	PROPERTIES
#	COMPILE_FLAGS "-DTEST_UTILS"
#	)
#
#
#ADD_EXECUTABLE(test_elf
#	${COMMON}
#	${ELF}
#	test_elf.c
#	)
#
#ADD_EXECUTABLE(test_ptrace
#	${PTRACE}
#	${COMMON}
#	${PROC}
#	${ELF}
#	test_ptrace.c
#	)
#
#ADD_EXECUTABLE(test_proc
#	${PROC}
#	${COMMON}
#	)
#
#SET_TARGET_PROPERTIES(test_proc
#	PROPERTIES
#	COMPILE_FLAGS "-DTEST_PROCUTILS"
#	)
#
#ADD_EXECUTABLE(test_wrapper
#	${PTRACE}
#	${COMMON}
#	${PROC}
#	${ELF}
#	test_wrapper.c
#	)
#
#
#ADD_EXECUTABLE(test_wrapper_2
#	${PTRACE}
#	${COMMON}
#	${PROC}
#	${ELF}
#	wrapper.c
#	syscall_wrapper_entrance.so
#	)

ADD_EXECUTABLE(currf2
	${COMMON}
	${PTRACE}
	${PROC}
	${ELF}
	currf2.c
	currf2_args.c
	checkpoint.h
	checkpoint.c
#	syscall_wrapper_entrance.so
	)
#
#SET(INJECTSO_SRC_FILES
#	syscall_wrapper_entrance.S
#	syscall_wrapper.c
#	test_wrapper_so.c
#	vsprintf.c
#	string_32.c
#	__ctype.c
#	__ctype.h
#	)
#
#ADD_CUSTOM_COMMAND(OUTPUT syscall_wrapper_entrance.so
#	COMMAND make -f makeso
#	MAIN_DEPENDENCY syscall_wrapper_entrance.S
#	DEPENDS ${INJECTSO_SRC_FILES}
#	WORKING_DIRECTORY ${CURRF2_SOURCE_DIR}
#	VERBATIM
#	)
#

SET(INJECTSO_GEN_FILES
	syscall_wrapper_entrance.o
	syscall_wrapper.o
	test_wrapper_so.o
	libprintf.o
	libprintf.os
	test_wrapper_so
	vsprintf.os
	string_32.os
	__ctype.os
	)

SET_DIRECTORY_PROPERTIES(PROPERTIES
	ADDITIONAL_MAKE_CLEAN_FILES "${INJECTSO_GEN_FILES}"
	)

INCLUDE(CMakeDependentOption)
INCLUDE(CheckIncludeFiles)
INCLUDE(CheckFunctionExists)
INCLUDE(CheckSymbolExists)

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


