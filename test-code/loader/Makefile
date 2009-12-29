all: loader.so test


loader.so: loader.o
	ld -shared $< -o $@

loader.o: loader.c
	gcc -c -fpic $< -o $@

test: test.o
	gcc -Wl,-dynamic-linker,$(shell pwd)/loader.so $< -o $@

.PHONY: clean

clean:
	rm -f *.o *.so test

