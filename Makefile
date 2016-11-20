CC=gcc
DEFS=-D_XOPEN_SOURCE=500 -D_BSD_SOURCE
CFLAGS=-Wall -g -std=c99 -pedantic $(DEFS)
#LDFLAGS=-lcrypt

all: mygzip

mygzip: mygzip.o
	$(CC) $(LDFLAGS) -o $@ $^

mygzip.o: mygzip.c
	$(CC) $(CFLAGS) -c -o $@ $<

test: mygzip
	./test.sh

clean:
	rm -f mygzip mygzip.o

.PHONY: clean
