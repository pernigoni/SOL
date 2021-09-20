CC = gcc

CFLAGS += -g -pedantic -Wall -I "../utils" -I "../conn" -I "../twrapper"

EXE = Signals/signals Toup_server_client/toup_server_sigwait Toup_server_client/toup_client utils/utils conn/conn twrapper/twrapper

OBJS = Toup_server_client/toup_server_sigwait.o Toup_server_client/toup_client.o utils/utils.o conn/conn.o twrapper/twrapper.o

.PHONY: clean

.SUFFIXES: .c .h

all : Signals/signals Toup_server_client/toup_server_sigwait Toup_server_client/toup_client

Signals/signals : Signals/signals.c utils/utils.o
	$(CC) $(CFLAGS) $^ -o $@

Toup_server_client/toup_client : Toup_server_client/toup_client.c utils/utils.o conn/conn.o -lpthread
	$(CC) $(CFLAGS) $^ -o $@

Toup_server_client/toup_server_sigwait : Toup_server_client/toup_server_sigwait.c utils/utils.o conn/conn.o twrapper/twrapper.o -lpthread
	$(CC) $(CFLAGS) $^ -o $@

clean :
	rm -f $(OBJS) $(EXE) *~ core