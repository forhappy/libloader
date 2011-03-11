
lib_LIBRARIES = libcommon.a
libcommon_a_SOURCES = defs.h debug.h debug.c exception.c exception.h utils.c utils.h \
					   ptraceutils.h ptraceutils.c elfutils.c elfutils.h \
					   procutils.h procutils.c ckptutils.c ckptutils.h

bin_PROGRAMS = snitchaser
snitchaser_SOURCES = snitchaser.c 


AM_CFLAGS = -g -std=gnu99 -O2 -Wall -D_GNU_SOURCE -D_FILE_OFFSET_BITS=64
printlog_CFLAGS = $(AM_CFLAGS) -DSYSCALL_PRINTER

