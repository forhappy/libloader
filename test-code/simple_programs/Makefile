TARGETS=simple

all: $(TARGETS)

simple: simple.o
	ld -shared -dynamic-linker /home/wn/work/snitchaser/build/src/interp/libinterp.so $^ -o $@
simple.o: simple.S

.PHONY: clean

clean:
	rm *.o $(TARGETS)

