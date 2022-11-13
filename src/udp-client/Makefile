GCC=cc
CFLAGS+=-Wall -g
CFLAGS+=-DUSE_SLEEP
SRC=$(wildcard *.c)
PROGS=$(patsubst %.c,%,$(SRC))

all: $(PROGS)

%.o: %.c
	$(GCC) $(CFLAGS+) $< -o $@

clean:	
	rm -f *.core $(PROGS) *~
