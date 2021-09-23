CC = gcc

CFLAGS += -g -pedantic -Wall -I "../utils" -I "../conn" -I "../twrapper" -I "../threadpool"

EXE = Es1/server Es1/toup_client utils/utils conn/conn twrapper/twrapper threadpool/threadpool

OBJS = Es1/server.o Es1/toup_client.o utils/utils.o conn/conn.o twrapper/twrapper.o threadpool/threadpool.o

.PHONY: clean

.SUFFIXES: .c .h

all : Es1/server Es1/toup_client

Es1/toup_client : Es1/toup_client.c utils/utils.o conn/conn.o twrapper/twrapper.o threadpool/threadpool.o -lpthread
	$(CC) $(CFLAGS) $^ -o $@

Es1/server : Es1/server.c utils/utils.o conn/conn.o twrapper/twrapper.o threadpool/threadpool.o -lpthread
	$(CC) $(CFLAGS) $^ -o $@

clean :
	rm -f $(OBJS) $(EXE) *~ core