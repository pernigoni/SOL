CC = gcc

CFLAGS += -g -pedantic -Wall -I "../Utils" -I "../Conn"

EXE = Bc_server_client/bc_server Bc_server_client/bc_client Toup_server_client/toup_server Toup_server_client/toup_client Toup_server_client/toup_server_select Utils/utils Conn/conn

OBJS = Bc_server_client/bc_server.o Bc_server_client/bc_client.o Toup_server_client/toup_server.o Toup_server_client/toup_client.o Toup_server_client/toup_server_select.o Utils/utils.o Conn/conn.o

.PHONY: clean

.SUFFIXES: .c .h

all : Bc_server_client/bc_server Bc_server_client/bc_client Toup_server_client/toup_server Toup_server_client/toup_client Toup_server_client/toup_server_select

Bc_server_client/bc_client : Bc_server_client/bc_client.c Utils/utils.o Conn/conn.o -lpthread # la roba con i thread è in Utils, non ho voglia di cancellarla
	$(CC) $(CFLAGS) $^ -o $@

Bc_server_client/bc_server : Bc_server_client/bc_server.c Utils/utils.o Conn/conn.o -lpthread # la roba con i thread è in Utils, non ho voglia di cancellarla
	$(CC) $(CFLAGS) $^ -o $@

Toup_server_client/toup_client : Toup_server_client/toup_client.c Utils/utils.o Conn/conn.o -lpthread
	$(CC) $(CFLAGS) $^ -o $@

Toup_server_client/toup_server : Toup_server_client/toup_server.c Utils/utils.o Conn/conn.o -lpthread
	$(CC) $(CFLAGS) $^ -o $@

Toup_server_client/toup_server_select : Toup_server_client/toup_server_select.c Utils/utils.o Conn/conn.o -lpthread
	$(CC) $(CFLAGS) $^ -o $@

clean :
	-rm -f $(OBJS) $(EXE) *~ core Bc_server_client/cs_sock Toup_server_client/cs_sock 