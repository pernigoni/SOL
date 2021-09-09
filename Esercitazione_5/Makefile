CC = gcc

CFLAGS += -g -pedantic -Wall -I "../Utils"

EXE = Myfind/myfind Utils/utils

OBJS = Myfind/myfind.o Utils/utils.o

.PHONY: clean

.SUFFIXES: .c .h

all: Myfind/myfind

Myfind/myfind : Myfind/myfind.c Utils/utils.o
	$(CC) $(CFLAGS) $^ -o $@

clean :
	-rm -f $(OBJS) $(EXE) *~ core