#
# a very simple Makefile for miniLZO
#
# Copyright (C) 1996-2010 Markus F.X.J. Oberhumer
#

PROGRAM = testmini
SOURCES = testmini.c minilzo.c

# Make sure that minilzo.h, lzoconf.h and lzodefs.h are in the
# current dircectory. Otherwise you may want to adjust CPPFLAGS.
##CPPFLAGS = -I../include/lzo -I.

GCC_CFLAGS = -s -Wall -O2 -fomit-frame-pointer

all:
	gcc $(CPPFLAGS) $(GCC_CFLAGS) -o $(PROGRAM) $(SOURCES)

cc:
	cc $(CPPFLAGS) -o $(PROGRAM) $(SOURCES)

clean:
	rm -f $(PROGRAM) $(PROGRAM).exe $(PROGRAM).map $(PROGRAM).tds
	rm -f *.err *.o *.obj

.PHONY:clean

