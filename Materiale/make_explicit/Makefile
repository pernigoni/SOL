CC = gcc

CFLAGS = -Wall -pedantic -I ./includes/

EXE = bin/programma

OBJS = objs/programma.o objs/toupper.o objs/stampa.o 

.PHONY : clean

$(EXE) : $(OBJS) 
	$(CC) $(CFLAGS) $^ -o $(EXE)

objs/stampa.o : src/stampa.c includes/stampa.h
	$(CC) $(CFLAGS) -c $< -o objs/stampa.o

objs/programma.o : src/programma.c includes/toupper.h includes/stampa.h
	$(CC) $(CFLAGS) -c $< -o objs/programma.o

objs/toupper.o : src/toupper.c includes/toupper.h
	$(CC) $(CFLAGS) -c $< -o objs/toupper.o

clean :
	-rm $(OBJS) $(EXE) *~ core

