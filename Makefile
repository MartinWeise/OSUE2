#
# \file Makefile
# \author Martin Weise <e1429167@student.tuwien.ac.at>
# \date 22.11.2016
# \brief The Makefile.
# \details Add -DENDEBUG to \p DEFS to enable debugging.
#

CC=gcc
DEFS=-D_XOPEN_SOURCE=500 -D_BSD_SOURCE
CFLAGS=-Wall -g -std=c99 -pedantic $(DEFS)
#LDFLAGS=-lcrypt

all: src/mygzip

src/mygzip: src/mygzip.o
	$(CC) $(LDFLAGS) -o $@ $^

src/mygzip.o: src/mygzip.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f src/mygzip src/mygzip.o

.PHONY: clean
