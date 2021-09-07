CC = gcc

CFLAGS += -g -pedantic -Wall -I "../Utils"

EXE = Passwd/passwd Matrix/read_matrix Matrix/write_matrix Wc/word_count

OBJS = Passwd/passwd.o Matrix/read_matrix.o Matrix/write_matrix.o Wc/word_count.o

.PHONY: clean

.SUFFIXES: .c .h

all: Passwd/passwd Matrix/read_matrix Matrix/write_matrix Wc/word_count

Passwd/passwd : Passwd/passwd.c Utils/utils.o
	$(CC) $(CFLAGS) $^ -o $@

Matrix/read_matrix : Matrix/read_matrix.c Utils/utils.o
	$(CC) $(CFLAGS) $^ -o $@

Matrix/write_matrix : Matrix/write_matrix.c Utils/utils.o
	$(CC) $(CFLAGS) $^ -o $@

Wc/word_count : Wc/word_count.c Utils/utils.o
	$(CC) $(CFLAGS) $^ -o $@

clean :
	-rm -f $(OBJS) $(EXE) *~ core Utils/utils.o
