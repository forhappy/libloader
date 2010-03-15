TARGETS=simple

all: $(TARGETS)

simple: simple.o
	ld $^ -o $@
simple.o: simple.S

.PHONY: clean

clean:
	rm *.o $(TARGETS)

