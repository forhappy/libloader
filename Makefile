all:libinterp.so test-suite

liblogckpt.os:
	$(MAKE) -C syscall

#libwrapper.so:
#	$(MAKE) -C libwrapper

CFLAGS += -g -std=gnu99 -Wall -Ilinux-include/include -Ilinux-include/arch/x86 \
-Ilinux-include/arch-generated/x86/generated -include \
linux-include/arch-generated/x86/generated/autoconf.h -D__KERNEL__ \
-DCONFIG_AS_CFI=1 -DCONFIG_AS_CFI_SIGNAL_FRAME=1 -Wno-unused \
-Wno-strict-aliasing -DINTERP_START_ENTRY -DELF32CLASS  -nostartfiles \
-nodefaultlibs -nostdlib -fno-builtin -fvisibility=hidden -fPIC 

libinterp.so:interp_main.o wrapper.o  snitchaser_main.o malloc.o debug.o  fdbnprintf.o processor.o exception.o startup_stack.o env_conf.o elf.o syscall.o libprintf.os liblogckpt.os proc.o
	ld -shared $^ -o $@


#startup.o:startup.S
#	gcc -c $^ -o $@

interp_main.o:interp_main.c 
	gcc $(CFLAGS) -o $@ $^
wrapper.o:wrapper.c
snitchaser_main.o:snitchaser_main.c
malloc.o:malloc.h malloc.c
vsprintf.o:vsprintf.c
string_32.o:string_32.c
__ctype.o:__ctype.c
libprintf.os:vsprintf.o string_32.o __ctype.o
	ld -r $^ -o $@
processor.o:processor.c 
debug.o:debug.c 
fdbnprintf.o:fdbnprintf.c 
exception.o:exception.c 
elf.o:elf.c 
startup_stack.o:startup_stack.c 
env_conf.o:env_conf.c 
syscall.o:syscall.c 
proc.o:proc.c 
# test-suite
test-suite:
	cd test/ && $(MAKE) 

.PHONY:clean
clean:
	rm *.o *.so *.os 
