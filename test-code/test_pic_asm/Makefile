all: main.so

func.o: func.c
	gcc -c -fPIC $^ -o $@

caller.o: caller.S
	gcc -c -fPIC $^ -o $@

main.so: caller.o func.o
	gcc -shared $^ -o $@

.PHONY: clean

clean:
	rm -f *.o main.so

