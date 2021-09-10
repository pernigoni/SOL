CC = gcc

CFLAGS += -g -pedantic -Wall -I "../Utils"

EXE = Myfind/myfind Mycp/mycp_sc Mycp/mycp_std Utils/utils

OBJS = Myfind/myfind.o Mycp/mycp_sc.o Mycp/mycp_std.o Utils/utils.o

.PHONY: clean

.SUFFIXES: .c .h

all: Myfind/myfind Mycp/mycp_sc Mycp/mycp_std

Myfind/myfind : Myfind/myfind.c Utils/utils.o
	$(CC) $(CFLAGS) $^ -o $@

Mycp/mycp_sc : Mycp/mycp_sc.c Utils/utils.o
	$(CC) $(CFLAGS) $^ -o $@

Mycp/mycp_std : Mycp/mycp_std.c Utils/utils.o
	$(CC) $(CFLAGS) $^ -o $@

clean :
	-rm -f $(OBJS) $(EXE) *~ core