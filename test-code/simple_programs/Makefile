TARGETS=simple.exe simple2.exe simple3 simple4.exe simple5 simple6.exe
CFLAGS=-g -O0


all: $(TARGETS)

%.exe: %.o
	ld $^ -o $@

#simple.exe: simple.o
#simple.o: simple.S
#simple2.exe: simple2.o
#simple2.o: simple2.S

.PHONY: clean

clean:
	rm -f *.o $(TARGETS)

