CC = gcc

CFLAGS += -g -pedantic -Wall -I "../Utils"

EXE = Prod_cons/prod_cons Calculator/calculator Pipe_bid/pipe_bid Utils/utils UQueue/queue

OBJS = Prod_cons/prod_cons.o Calculator/calculator.o Pipe_bid/pipe_bid.o Utils/utils.o UQueue/queue.o

.PHONY: clean

.SUFFIXES: .c .h

all : Prod_cons/prod_cons Calculator/calculator Pipe_bid/pipe_bid

Prod_cons/prod_cons : Prod_cons/prod_cons.c Utils/utils.o UQueue/queue.o -lpthread
	$(CC) $(CFLAGS) -I "../UQueue" $^ -o $@

Calculator/calculator : Calculator/calculator.c Utils/utils.o -lpthread
	$(CC) $(CFLAGS) $^ -o $@

Pipe_bid/pipe_bid : Pipe_bid/pipe_bid.c Utils/utils.o -lpthread
	$(CC) $(CFLAGS) $^ -o $@

clean :
	-rm -f $(OBJS) $(EXE) *~ core