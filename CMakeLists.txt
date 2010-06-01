CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
SET(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMakeModules/")

INCLUDE(CMakeDependentOption)
INCLUDE(CheckIncludeFiles)
INCLUDE(CheckFunctionExists)
INCLUDE(CheckSymbolExists)


PROJECT(SNITCHASER)
# default build type
IF(NOT CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE Debug CACHE STRING
      "Choose the type of build, options are: Debug Release"
      FORCE)
ENDIF(NOT CMAKE_BUILD_TYPE)

# CMAKE_BUILD_TYPE can only be one of Debug or Release

SET(build_type_ok FALSE)
FOREACH(a "Debug" "Release")
	IF (${CMAKE_BUILD_TYPE} STREQUAL ${a})
		SET(build_type_ok TRUE)
	ENDIF (${CMAKE_BUILD_TYPE} STREQUAL ${a})
ENDFOREACH(a)

IF (NOT build_type_ok)
	MESSAGE(FATAL_ERROR "CMAKE_BUILD_TYPE can only be one of \"Debug\" or \"Release\"")
ENDIF (NOT build_type_ok)


IF (${SNITCHASER_BINARY_DIR} STREQUAL ${SNITCHASER_SOURCE_DIR})
	MESSAGE(FATAL_ERROR "In-tree-compile is not prefered.")
ENDIF (${SNITCHASER_BINARY_DIR} STREQUAL ${SNITCHASER_SOURCE_DIR})

IF (NOT CMAKE_COMPILER_IS_GNUCC)
	MESSAGE(FATAL_ERROR "Only accept GCC now")
ENDIF (NOT CMAKE_COMPILER_IS_GNUCC)

# set debug options
SET(MARCH_CFLAGS "-march=${MARCH} -mtune=${MARCH}")
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
		SET(KARCH "x86")
	ENDIF (ARCH STREQUAL ${a})
ENDFOREACH(a)
IF (NOT arch_set MATCHES "^yes$")
	MESSAGE(FATAL_ERROR "doesn't support ${ARCH}")
ENDIF (NOT arch_set MATCHES "^yes$")
unset(arch_set)

MACRO(check_kernel varname descstr checkstr checkpathlist checkfile)
SET(${varname} "auto" CACHE PATH ${descstr})
set (${ks_set} "no")
IF (${varname} STREQUAL "auto")
#  1) check /lib/modules/${CMAKE_HOST_SYSTEM_VERSION}/source
#  2) check /usr/src/linux
	FOREACH(c ${checkpathlist})
		IF (NOT ks_set STREQUAL "yes")
			MESSAGE(STATUS "check ${checkstr} from ${c}")
			EXECUTE_PROCESS(COMMAND stat ${c}/${checkfile}
				OUTPUT_QUIET
				RESULT_VARIABLE stat_res)
			IF (stat_res STREQUAL "0")
				SET(${varname} ${c} CACHE PATH ${descstr}
					FORCE)
				SET(ks_set "yes")
			ENDIF (stat_res STREQUAL "0")
		ENDIF (NOT ks_set STREQUAL "yes")
	ENDFOREACH (c)
ELSE (${varname} STREQUAL "auto")
	EXECUTE_PROCESS(COMMAND stat ${${varname}}/${checkfile}
		OUTPUT_QUIET
		RESULT_VARIABLE stat_res)
	IF (stat_res STREQUAL "0")
		SET(ks_set "yes")
	ENDIF (stat_res STREQUAL "0")
ENDIF (${varname} STREQUAL "auto")
IF (NOT ks_set STREQUAL "yes")
	MESSAGE(FATAL_ERROR "unable to find ${checkstr}")
ENDIF (NOT ks_set STREQUAL "yes")
unset(ks_set)
unset(stat_res)
ENDMACRO(check_kernel varname descstr checkpath)

check_kernel(KERNEL_SOURCE
	"kernel source tree, 'auto' means auto-detect"
	"kernel source tree"
	"/lib/modules/${CMAKE_HOST_SYSTEM_VERSION}/source;/usr/src/linux"
	Kbuild)
check_kernel(KERNEL_BUILD_SOURCE
	"kernel build tree, 'auto' means auto-detect"
	"kernel build tree"
	"/lib/modules/${CMAKE_HOST_SYSTEM_VERSION}/build;/usr/src/linux"
	include/linux/autoconf.h)

# build kernel related files cflags and include dir
EXECUTE_PROCESS(COMMAND gcc -print-file-name=include
	OUTPUT_VARIABLE gcc_inc
	OUTPUT_STRIP_TRAILING_WHITESPACE)
## check include2
EXECUTE_PROCESS(COMMAND stat ${KERNEL_BUILD_SOURCE}/include2
	OUTPUT_QUIET
	ERROR_QUIET
	RESULT_VARIABLE have_include2)
IF (have_include2 STREQUAL "0")
	# source and build tree are different
	SET(KERNEL_CFLAGS "-isystem ${gcc_inc} -I${KERNEL_BUILD_SOURCE}/include -I${KERNEL_BUILD_SOURCE}/include2 -I${KERNEL_SOURCE}/include -I${KERNEL_SOURCE}/arch/${KARCH}/include -include ${KERNEL_BUILD_SOURCE}/include/linux/autoconf.h -I${KERNEL_SOURCE}/arch/${KARCH}/kernel -I${KERNEL_BUILD_SOURCE}/arch/${KARCH}/kernel -D__KERNEL__ -DCONFIG_AS_CFI=1 -DCONFIG_AS_CFI_SIGNAL_FRAME=1 -Wno-unused -Wno-strict-aliasing")

ELSE (have_include2 STREQUAL "0")
	SET(KERNEL_CFLAGS "-isystem ${gcc_inc} -I${KERNEL_BUILD_SOURCE}/include -I${KERNEL_SOURCE}/arch/${KARCH}/include -include ${KERNEL_BUILD_SOURCE}/include/linux/autoconf.h -D__KERNEL__ -DCONFIG_AS_CFI=1 -DCONFIG_AS_CFI_SIGNAL_FRAME=1 -Wno-unused -Wno-strict-aliasing")
ENDIF (have_include2 STREQUAL "0")

unset(have_include2)
unset(gcc_inc)

###########################################################

SET(MARCH ${CMAKE_SYSTEM_PROCESSOR} CACHE STRING
	"gcc -march option")
# Build types
SET(MARCH_CFLAGS "-march=${MARCH} -mtune=${MARCH}")
SET(CMAKE_C_FLAGS "-std=gnu99 -Wall -Wextra -D_GNU_SOURCE ${MARCH_CFLAGS}" CACHE
	STRING "Flags used by the compiler during all build types." FORCE)
SET(CMAKE_C_FLAGS_DEBUG "-g -O0 -DSNITCHASER_DEBUG" CACHE
	STRING "Flags used by the compiler during debug builds." FORCE)
SET(CMAKE_C_FLAGS_RELEASE "-g -O2" CACHE
	STRING "Flags used by the compiler during release builds." FORCE)

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

FIND_PACKAGE(BISON)
FIND_PACKAGE(FLEX)

IF (!FLEX_FOUND)
	MESSAGE(FATAL_ERROR "flex not found")
ENDIF (!FLEX_FOUND)

IF (!BISON_FOUND)
	MESSAGE(FATAL_ERROR "flex not found")
ENDIF (!BISON_FOUND)


# Options

SET(BUILD_TESTS OFF CACHE BOOL
	"Whether to build tests")
IF ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
	SET(BUILD_TESTS ON CACHE BOOL
		"Whether to build tests" FORCE)
ENDIF ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")

SET(RELAX_SIGNAL OFF
	CACHE BOOL "don't block signals")

SET(INTERP_FILE "/lib/ld-linux.so.2"
	CACHE FILEPATH "interpreter so file")

SET(LOG_DIR "/tmp/snitchaser"
	CACHE PATH "log file dir")

# check environment

CONFIGURE_FILE(config.h.cmake.in config.h)

ADD_SUBDIRECTORY(src)

# vim:tabstop=4:shiftwidth=4

