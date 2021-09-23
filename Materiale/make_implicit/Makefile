CC = gcc

CFLAGS = -Wall -pedantic -I ./

EXE = programma

.PHONY : clean

$(EXE) : toupper.o stampa.o programma.o 

stampa.o : stampa.c stampa.h

programma.o : programma.c toupper.h stampa.h

toupper.o : toupper.c toupper.h

clean :
	-rm *.o $(EXE) *~ core

