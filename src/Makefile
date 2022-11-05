# ISA
# @file Makefile
# @authors	Matus Vrablik (xvrabl05)

CC = gcc
FLAGS = 	-g -pedantic -Wall -Wextra -lpcap
OTHER_FLAGS = -c
EXE_FILES = flow 
ZIP_NAME = xvrabl05
FILES = f_time.o f_handle.o
.PHONY: all clean zip

all: flow $(FILES)

# univerzalne pravidlo pre preklad .c suborov na .o
%.o: %.c
	$(CC) $(FLAGS) ${OTHER_FLAGS} $<

flow: flow.o $(FILES)
	$(CC) $(FLAGS) flow.o $(FILES) -o flow

# Commands
clean:
	rm -f $(EXE_FILES) *.o

zip:
	zip $(ZIP_NAME).zip *.c *.h Makefile

# Koniec suboru Makefile
