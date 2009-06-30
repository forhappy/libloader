all: pre-req libwrapper.so # test_libwrapper
	cp ./libwrapper.so ../

CFLAGS= -g -Wall -nostartfiles -nodefaultlibs -nostdlib -fno-builtin -fvisibility=hidden -fPIC  -I../  -I.

libwrapper.so: entry.o wrapper.o libprintf.os libcheckpoint.os
	ld -shared -e show_help_entry $^ -o $@
	@if ! readelf -r $@  | grep "no relocations" > /dev/null ; then echo "*** Warning *** $@ needs relocation" ; fi

entry.o: entry.S
	as $^ -o $@

wrapper.o: wrapper.c injector.h

#test_libwrapper: entry.o wrapper.o libprintf.os test_libwrapper.o
#	gcc -g -Wl,-rpath,. $^ -o $@

#test_libwrapper.o: test_libwrapper.c
#	gcc -g -c $^ -o $@

vsprintf.os: vsprintf.c
string_32.os: string_32.c
__ctype.os: __ctype.c
injector_utils.os: injector_utils.c injector_utils.h
injector_debug.os: injector_debug.c injector_debug.h

%.os : %.c
	gcc -fno-builtin $(CFLAGS) -c $< -o $@

libprintf.os: vsprintf.os string_32.os __ctype.os injector_utils.os injector_debug.os
	ld -r $^ -o $@

libcheckpoint.os:
	make -C ./checkpoint
	cp checkpoint/libcheckpoint.os .


.PHONY: clean pre-req

clean:
	rm -f *.o *.os *.so test_libwrapper
	make -C checkpoint clean

# vim:filetype=make

