CC=gcc
DEFS=-D_XOPEN_SOURCE=500 -D_BSD_SOURCE -DENDEBUG
CFLAGS=-Wall -g -std=c99 -pedantic $(DEFS)
#LDFLAGS=-lcrypt

all: src/mygzip

src/mygzip: src/mygzip.o
	$(CC) $(LDFLAGS) -o $@ $^

src/mygzip.o: src/mygzip.c
	$(CC) $(CFLAGS) -c -o $@ $<

test: all
	sh test/test.sh

run: all
	src/mygzip

doxygen: all
	sh doc/batch.sh

clean:
	rm -f src/mygzip src/mygzip.o

.PHONY: clean
