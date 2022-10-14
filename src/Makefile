# ISA
# @file Makefile
# @authors	Matus Vrablik (xvrabl05)

CC = gcc
FLAGS = -g -pedantic -Wall -Wextra -lpcap
OTHER_FLAGS = -c
EXE_FILES = flow
ZIP_NAME = xvrabl05
.PHONY: all clean zip

all: flow

# univerzalne pravidlo pre preklad .c suborov na .o
%.o: %.c
	$(CC) $(FLAGS) ${OTHER_FLAGS} $<

flow: flow.o
	$(CC) $(FLAGS) flow.o -o flow

# Commands
clean:
	rm -f flow *.o

zip:
	zip $(ZIP_NAME).zip *.c *.h Makefile

# Koniec suboru Makefile
