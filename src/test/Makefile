all:test-1 test-2

CFLAGS += -g -std=gnu99 -Wall 

test-1:test-1.o
	gcc -Wl,-dynamic-linker,$(shell pwd)/../libinterp.so $< -o $@
test-1.o:test-1.c

test-2:test-2.o
	gcc -Wl,-dynamic-linker,$(shell pwd)/../libinterp.so $< -o $@
test-2.o:test-2.c

.PHONY:clean
clean:
	rm *.o  test-1 test-2 

