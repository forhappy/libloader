all:test_excpt
CFLAGS += -g -std=gnu99 -Wall
test_excpt:test_excpt.o rb_exception.o rb_debug.o
	gcc -rdynamic -o $@ $^
test_excpt.o:test_excpt.c
	gcc -c $^ -o $@
rb_exception.o:rb_exception.c rb_exception.h rb_exception_types.h
	gcc -c $< -o $@
rb_debug.o:rb_debug.c rb_debug.h
	gcc -c $< -o $@
.PHONY:clean
clean:
	rm *.o test_excpt
