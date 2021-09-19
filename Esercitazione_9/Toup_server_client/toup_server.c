/*
      Per ogni nuova connessione il server lancia un thread che gestisce
      tutte le richieste del client.
      Modello "un thread per connessione", i thread sono spawnati in
      modalità detached.
*/
#include <assert.h>
#include <ctype.h>
#include "../Utils/utils.h"
#include "../Conn/conn.h"

typedef struct msg // messaggio
{
	int len;
	char *str;
} msg_t;

void cleanup() 
{
  	unlink(SOCKNAME);
}

void toup(char *str) // converte tutti i carattere minuscoli in caratteri maiuscoli
{
      char *p = str;
      while(*p != '\0') 
      { 
            *p = (islower(*p) ? toupper(*p) : *p); 
            p++;
      }        
}

void *thread_fun(void *arg) 
{
      assert(arg);
      long conn_fd = (long)arg, n;
      while(1)
      {
            msg_t msg;

            SYSCALL_EXIT(readn, n, readn(conn_fd, &msg.len, sizeof(int)), "readn.1", "");
            if(n == 0) 
                  break;
            msg.str = safe_calloc(msg.len, sizeof(char));
            SYSCALL_EXIT(readn, n, readn(conn_fd, msg.str, msg.len * sizeof(char)), "readn.2", "");
            // ho ricevuto il messaggio
            toup(msg.str);
            // invio la risposta al client
            SYSCALL_EXIT(writen, n, writen(conn_fd, &msg.len, sizeof(int)), "writen.1", "");
            SYSCALL_EXIT(writen, n, writen(conn_fd, msg.str, msg.len * sizeof(char)), "writen.2", "");
            free(msg.str);
      }
      int r;
      SYSCALL_EXIT(close, r, close(conn_fd), "close", "");
      printf("Connessione chiusa\n");
      return NULL;
}

void spawn_thread(long conn_fd) 
{
      pthread_t tid;
      pthread_attr_t attr; // (è il primo esercizio in cui servono gli attributi)
      
      int r;
      if(pthread_attr_init(&attr) != 0) 
      {
            perror("pthread_attr_init");
		SYSCALL_EXIT(close, r, close(conn_fd), "close", "");
            return ;
      }
      // setto il thread in modalità detached
      if(pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED) != 0) 
      {
            perror("pthread_attr_setdetachstate");
            pthread_attr_destroy(&attr);
		SYSCALL_EXIT(close, r, close(conn_fd), "close", "");
            return ;
      }
      // creo il thread
      if(pthread_create(&tid, &attr, thread_fun, (void *)conn_fd) != 0) 
      {
            perror("pthread_create");
            pthread_attr_destroy(&attr);
            SYSCALL_EXIT(close, r, close(conn_fd), "close", "");
            return ;
      }
}

int main(int argc, char *argv[]) 
{
      cleanup();    
      atexit(cleanup);    

      int listen_fd;
      SYSCALL_EXIT(socket, listen_fd, socket(AF_UNIX, SOCK_STREAM, 0), "socket", "");

      struct sockaddr_un serv_addr;
      memset(&serv_addr, '0', sizeof(serv_addr));
      serv_addr.sun_family = AF_UNIX;    
      strncpy(serv_addr.sun_path, SOCKNAME, strlen(SOCKNAME) + 1);

      int not_used;
      SYSCALL_EXIT(bind, not_used, bind(listen_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)), "bind", "");
      SYSCALL_EXIT(listen, not_used, listen(listen_fd, MAXBACKLOG), "listen", "");

      long conn_fd;
      while(1)
      {
            SYSCALL_EXIT(accept, conn_fd, accept(listen_fd, (struct sockaddr *)NULL ,NULL), "accept", "");
            spawn_thread(conn_fd);
      }
      return 0;
}