CC = gcc

CFLAGS += -g -pedantic -Wall -I "../Utils"

EXE = Produttore_consumatore/produttore_consumatore Filosofi/filosofi Pipeline_3_threads/pipeline_3_threads Pipeline_hash/pipeline_hash Utils/utils BQueue/boundedqueue icl_hash/icl_hash

OBJS =  Produttore_consumatore/produttore_consumatore.o Filosofi/filosofi.o Pipeline_3_threads/pipeline_3_threads.o Pipeline_hash/pipeline_hash.o Utils/utils.o BQueue/boundedqueue.o icl_hash/icl_hash.o

.PHONY: clean

.SUFFIXES: .c .h

all: Produttore_consumatore/produttore_consumatore Filosofi/filosofi Pipeline_3_threads/pipeline_3_threads Pipeline_hash/pipeline_hash

Produttore_consumatore/produttore_consumatore : Produttore_consumatore/produttore_consumatore.c Utils/utils.o -lpthread
	$(CC) $(CFLAGS) $^ -o $@

Filosofi/filosofi : Filosofi/filosofi.c Utils/utils.o -lpthread
	$(CC) $(CFLAGS) $^ -o $@

Pipeline_3_threads/pipeline_3_threads : Pipeline_3_threads/pipeline_3_threads.c Utils/utils.o BQueue/boundedqueue.o -lpthread
	$(CC) $(CFLAGS) -I "../BQueue" $^ -o $@

Pipeline_hash/pipeline_hash : Pipeline_hash/pipeline_hash.c Utils/utils.o BQueue/boundedqueue.o icl_hash/icl_hash.o -lpthread
	$(CC) $(CFLAGS) -I "../BQueue" -I "../icl_hash" $^ -o $@

clean :
	-rm -f $(OBJS) $(EXE) *~ core