CC = gcc

CFLAGS += -g -pedantic -Wall

EXE = Passwd/passwd Matrix/read_matrix Matrix/write_matrix word_count

OBJS = Passwd/passwd.o Matrix/read_matrix.o Matrix/write_matrix.o word_count.o

.PHONY: clean

.SUFFIXES: .c .h

all: Passwd/passwd Matrix/read_matrix Matrix/write_matrix word_count

Passwd/passwd : Passwd/passwd.c
	$(CC) $(CFLAGS) $^ -o $@

Matrix/read_matrix : Matrix/read_matrix.c
	$(CC) $(CFLAGS) $^ -o $@

Matrix/write_matrix : Matrix/write_matrix.c
	$(CC) $(CFLAGS) $^ -o $@

word_count : word_count.c
	$(CC) $(CFLAGS) $^ -o $@

clean :
	-rm -f $(OBJS) $(EXE) *~ core