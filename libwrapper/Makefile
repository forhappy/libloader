all: pre-req libwrapper.so test_libwrapper
	cp ./libwrapper.so ../

CFLAGS= -g -Wall -nostartfiles -nodefaultlibs -nostdlib -fno-builtin -fvisibility=hidden -fPIC 

libwrapper.so: entry.o wrapper.o libprintf.os
	ld -shared -e show_help_entry $^ -o $@
	@if ! readelf -r $@  | grep "no relocations" > /dev/null ; then echo "*** Warning *** $@ needs relocation" ; fi

entry.o: entry.S
	as $^ -o $@

wrapper.o: wrapper.c injector.h

test_libwrapper: entry.o wrapper.o libprintf.os test_libwrapper.o
	gcc -g -Wl,-rpath,. $^ -o $@

test_libwrapper.o: test_libwrapper.c
	gcc -g -c $^ -o $@

vsprintf.os: vsprintf.c
string_32.os: string_32.c
__ctype.os: __ctype.c
utils.os: utils.c

%.os : %.c
	gcc -fno-builtin $(CFLAGS) -c $^ -o $@

libprintf.os: vsprintf.os string_32.os __ctype.os utils.os
	ld -r $^ -o $@

.PHONY: clean pre-req

clean:
	rm -f *.o *.os

pre-req: config.h defs.h

config.h:
	@if ! test -e ./config.h ; then ln -s ../config.h .; fi

defs.h:
	@if ! test -e ./defs.h ; then ln -s ../defs.h .; fi


# vim:filetype=make

