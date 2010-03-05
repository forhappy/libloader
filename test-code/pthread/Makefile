CFLAGS=-g -std=gnu99
LDFLAGS=-lpthread

all: test_pthread test_tls

test_pthread: test_pthread.o
test_tls: test_tls.o

.PHONY: clean

clean:
	rm -f *.o test_pthread test_tls

