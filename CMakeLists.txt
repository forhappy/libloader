CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

INCLUDE(CMakeDependentOption)
INCLUDE(CheckIncludeFiles)
INCLUDE(CheckFunctionExists)
INCLUDE(CheckSymbolExists)


PROJECT(SNITCHASER)

IF (${SNITCHASER_BINARY_DIR} STREQUAL ${SNITCHASER_SOURCE_DIR})
	MESSAGE(FATAL_ERROR "In-tree-compile is not prefered.")
ENDIF (${SNITCHASER_BINARY_DIR} STREQUAL ${SNITCHASER_SOURCE_DIR})

IF (NOT CMAKE_COMPILER_IS_GNUCC)
	MESSAGE(FATAL_ERROR "Only accept GCC now")
ENDIF (NOT CMAKE_COMPILER_IS_GNUCC)


# detect architecture

SET(ARCH "auto" CACHE STRING "target architecture, 'auto' means auto-detect")

IF (ARCH MATCHES "^auto$")
	FOREACH(a "i386" "i686" "x86")
		IF (${CMAKE_SYSTEM_PROCESSOR} STREQUAL ${a})
			SET(ARCH "x86" CACHE STRING "target architecture" FORCE)
		ENDIF (${CMAKE_SYSTEM_PROCESSOR} STREQUAL ${a})
	ENDFOREACH(a)
ENDIF (ARCH MATCHES "^auto$")


set(arch_set "no")
FOREACH (a "x86")
	IF (ARCH STREQUAL ${a})
		set(arch_set "yes")
	ENDIF (ARCH STREQUAL ${a})
ENDFOREACH(a)
IF (NOT arch_set MATCHES "^yes$")
	MESSAGE(FATAL_ERROR "doesn't support ${ARCH}")
ENDIF (NOT arch_set MATCHES "^yes$")
unset(arch_set)

# find kernel source

SET(KERNEL_SOURCE "auto" CACHE FILEPATH "kernel source tree, 'auto' means auto-detect")
set (${ks_set} "no")
IF (KERNEL_SOURCE STREQUAL "auto")
#  1) check /lib/modules/${CMAKE_HOST_SYSTEM_VERSION}/source
#  2) check /usr/src/linux
	FOREACH(c "/lib/modules/${CMAKE_HOST_SYSTEM_VERSION}/source"
			"/usr/src/linux")
		IF (NOT ks_set STREQUAL "yes")
			MESSAGE(STATUS "check kernel source from ${c}")
			EXECUTE_PROCESS(COMMAND stat ${c}/Kbuild
				OUTPUT_FILE /dev/null
				RESULT_VARIABLE stat_res)
			IF (stat_res STREQUAL "0")
				SET(KERNEL_SOURCE ${c} CACHE FILEPATH "kernel source tree, 'auto' means auto-detect"
					FORCE)
				SET(ks_set "yes")
			ENDIF (stat_res STREQUAL "0")
		ENDIF (NOT ks_set STREQUAL "yes")
	ENDFOREACH (c)
ELSE (KERNEL_SOURCE STREQUAL "auto")
	EXECUTE_PROCESS(COMMAND stat ${KERNEL_SOURCE}/Kbuild
		OUTPUT_FILE /dev/null
		RESULT_VARIABLE stat_res)
	IF (stat_res STREQUAL "0")
		SET(ks_set "yes")
	ENDIF (stat_res STREQUAL "0")
ENDIF (KERNEL_SOURCE STREQUAL "auto")

IF (NOT ks_set STREQUAL "yes")
	MESSAGE(FATAL_ERROR "unable to find kernel source tree")
ENDIF (NOT ks_set STREQUAL "yes")
unset(ks_set)
unset(stat_res)


SET(MARCH ${CMAKE_SYSTEM_PROCESSOR} CACHE STRING
	"gcc -march option")
# Build types
SET(MARCH_CFLAGS "-march=${MARCH} -mtune=${MARCH}")
SET(CMAKE_C_FLAGS "-std=gnu99 -Wall -D_GNU_SOURCE ${MARCH_CFLAGS}")
SET(CMAKE_C_FLAGS_DEBUG "-g -O0 -DSNITCHASER_DEBUG")
SET(CMAKE_C_FLAGS_RELEASE "-O2")

# Environment
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
SET(CMAKE_REQUIRED_LIBRARIES	"-lrt")
LIST(APPEND LINK_LIBRARIES "-lrt")
CHECK_FUNCTION_EXISTS(clock_gettime HAVE_CLOCK_GETTIME)


# Options

SET(BUILD_TESTS OFF CACHE BOOL
	"Whether to build tests")
IF ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
	SET(BUILD_TESTS ON CACHE BOOL
		"Whether to build tests" FORCE)
ENDIF ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")

# check environment

CONFIGURE_FILE(config.h.cmake.in config.h)

ADD_SUBDIRECTORY(src)

# vim:tabstop=4:shiftwidth=4

