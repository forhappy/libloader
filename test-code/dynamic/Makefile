all: loader

test.so: test.o
	ld -shared $< -o $@

test.o: test.c
	gcc -g -c -fPIC $< -o $@

loader: loader.o test.so
	gcc -g $^ -Wl,-rpath=$(shell pwd) -o $@

loader.o: loader.c
	gcc -g -c $< -o $@

.PHONY: clean

clean:
	rm *.o *.so loader -f

