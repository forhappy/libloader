TARGETS=simple simple2

all: $(TARGETS)

simple: simple.o
	ld -shared -dynamic-linker /home/wn/work/snitchaser/build/src/interp/libinterp.so $^ -o $@
simple.o: simple.S

simple2: simple2.o
	ld -shared -dynamic-linker /home/wn/work/snitchaser/build/src/interp/libinterp.so $^ -o $@
simple2.o: simple2.S

.PHONY: clean

clean:
	rm *.o $(TARGETS)

