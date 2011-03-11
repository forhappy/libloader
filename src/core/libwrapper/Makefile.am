lib_LTLIBRARIES = libwrapper.la
libwrapper_la_SOURCES = entry.S wrapper.c signal.c injector.h __ctype.c \
						injector_utils.c injector_utils.h injector_debug.c \
						injector_debug.h
libwrapper_la_LIBADD = ../checkpoint/libcheckpoint.a
AM_CFLAGS = -g -std=gnu99 -Wall -nostartfiles -nodefaultlibs -nostdlib -fno-builtin -fPIC \
			-DRELAX_SINGAL -DFORK_CKPT 
libwrapper_la_CFLAGS=$(AM_CFLAGS)
