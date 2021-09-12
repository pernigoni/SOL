CC = gcc

CFLAGS += -g -pedantic -Wall -I "../Utils"

EXE = Dummyshell/dummyshell Background/background Zombie/zombie Family/family Fib13/fib13 Utils/utils

OBJS =  Dummyshell/dummyshell.o Background/background.o Zombie/zombie.o Family/family.o Fib13/fib13.o Utils/utils.o

.PHONY: clean

.SUFFIXES: .c .h

all: Dummyshell/dummyshell Background/background Zombie/zombie Family/family Fib13/fib13

Dummyshell/dummyshell : Dummyshell/dummyshell.c Utils/utils.o
	$(CC) $(CFLAGS) $^ -o $@

Background/background : Background/background.c Utils/utils.o
	$(CC) $(CFLAGS) $^ -o $@

Zombie/zombie : Zombie/zombie.c Utils/utils.o
	$(CC) $(CFLAGS) $^ -o $@

Family/family : Family/family.c Utils/utils.o
	$(CC) $(CFLAGS) $^ -o $@

Fib13/fib13 : Fib13/fib13.c Utils/utils.o
	$(CC) $(CFLAGS) $^ -o $@

clean :
	-rm -f $(OBJS) $(EXE) *~ core