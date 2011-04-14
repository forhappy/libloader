all:libinterp.so test-suite

liblogckpt.os:
	$(MAKE) -C syscall

#libwrapper.so:
#	$(MAKE) -C libwrapper

CFLAGS += -g -std=gnu99 -Wall -DINTERP_START_ENTRY -DELF32CLASS \
		  -nostartfiles -nodefaultlibs -nostdlib -fno-builtin -fvisibility=hidden -fPIC 

libinterp.so: interp_main.o wrapper.o  snitchaser_main.o malloc.o debug.o  fdbnprintf.o processor.o exception.o startup_stack.o env_conf.o elf.o syscall.o libprintf.os liblogckpt.os proc.o
	ld -shared $^ -o $@


#startup.o:startup.S
#	gcc -c $^ -o $@

interp_main.o:interp_main.c 
wrapper.o:wrapper.c
snitchaser_main.o:snitchaser_main.c
malloc.o:malloc.h malloc.c
vsprintf.o:vsprintf.c
string_32.o:string_32.c
__ctype.o:__ctype.c
libprintf.os:vsprintf.o string_32.o __ctype.o
	ld -r $^ -o $@
processor.o:processor.c processor.h
debug.o:debug.c debug.h debug_components.h
fdbnprintf.o:fdbnprintf.c fdbnprintf.h
exception.o:exception.c exception.h exception_types.h
elf.o:elf.c elf.h
startup_stack.o:startup_stack.c startup_stack.h
env_conf.o:env_conf.c env_conf.h env_conf_entries.h
syscall.o:syscall.c syscall.h
proc.o:proc.c proc.h

test-suite:
	cd test/ && $(MAKE) 


.PHONY:clean
clean:
	rm *.o *.so *.os 
