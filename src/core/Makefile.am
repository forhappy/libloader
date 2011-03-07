SUBDIRS = checkpoint libwrapper
lib_LIBRARIES = libcommon.a
libcommon_a_SOURCES = defs.h debug.h debug.c exception.c exception.h utils.c utils.h \
					   ptraceutils.h ptraceutils.c elfutils.c elfutils.h \
					   procutils.h procutils.c ckptutils.c ckptutils.h
bin_PROGRAMS =snitchaser gdbloader printlog

snitchaser_SOURCES = snitchaser.c snitchaser_args.c
snitchaser_LDADD = libwrapper/libwrapper.la libcommon.a checkpoint/libcheckpoint.a

gdbloader_SOURCES = gdbloader.c gdbloader_args.c
gdbloader_LDADD = libcommon.a 

printlog_SOURCES = printlog.c
printlog_LDADD = libcommon.a checkpoint/libprinter.a

AM_CFLAGS = -g -std=gnu99 -O2 -Wall -D_GNU_SOURCE -D_FILE_OFFSET_BITS=64 
printlog_CFLAGS = $(AM_CFLAGS) -DSYSCALL_PRINTER
