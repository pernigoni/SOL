/*
      Per ogni nuova connessione il server lancia un thread che gestisce
      tutte le richieste del client.
      Modello "un thread per connessione", i thread sono spawnati in
      modalità detached.

      Gestione dei segnali in modo sincrono utilizzando un thread dedicato.
*/
#include <assert.h>
#include <ctype.h>
#include <signal.h>
#include "../utils/utils.h"
#include "../twrapper/twrapper.h"
#include "../conn/conn.h"

int end = 0, listen_fd; // invece di essere globali andrebbero passate al thread per riferimento nell'argomento

typedef struct msg // messaggio
{
	int len;
	char *str;
} msg_t;

void toup(char *str) // converte tutti i carattere minuscoli in caratteri maiuscoli
{
      char *p = str;
      while(*p != '\0') 
      { 
            *p = (islower(*p) ? toupper(*p) : *p); 
            p++;
      }        
}

void *sighandler_fun(void *arg)
{
      assert(arg);
      sigset_t *mask = (sigset_t *)arg;

      while(1)
      {
            int   sig,
                  r = sigwait(mask, &sig);
            if(r != 0)
            {
                  errno = r;
                  perror("sigwait");
                  return NULL;
            }

            switch(sig)
            {     
                  case SIGINT:
                  case SIGTERM:
                  case SIGQUIT:
                        end = 1;
                        shutdown(listen_fd, SHUT_RDWR);
            /*
            SOLUZIONE ALTERNATIVA CHE NON USA shutdown
                  Il thread dispatcher (in questo esercizio il thread main), invece di sospendersi su una 
                  accept, si sospende su una select in cui, oltre al listen socket, registra il descrittore
                  di lettura di una pipe condivisa con il thread sighandler. Il thread sighandler quando
                  riceve il segnale chiude il descrittore di scrittura in modo da sbloccare la select.
            */
                        return NULL;
                  default: ;
            }
      }
      return NULL;
}

void *thread_fun(void *arg) 
{
      assert(arg);
      long conn_fd = (long)arg;
      int n;
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
      int r;
      pthread_t tid;
      pthread_attr_t attr;
      //----------------------------------------//
      sigset_t mask, old_mask;
      sigemptyset(&mask);
      sigaddset(&mask, SIGINT); 
      sigaddset(&mask, SIGQUIT);

      if(pthread_sigmask(SIG_BLOCK, &mask, &old_mask) != 0) 
      {
            perror("pthread_sigmask");
            SYSCALL_EXIT(close, r, close(conn_fd), "close", "");
            return ;
      }
      //----------------------------------------//
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
      //----------------------------------------//
      if(pthread_sigmask(SIG_SETMASK, &old_mask, NULL) != 0) 
      {
	      perror("pthread_sigmask");
	      SYSCALL_EXIT(close, r, close(conn_fd), "close", "");
      }
}

int main(int argc, char *argv[]) 
{
      sigset_t mask;
      sigemptyset(&mask);
      sigaddset(&mask, SIGINT);
      sigaddset(&mask, SIGQUIT);
      sigaddset(&mask, SIGTERM);
      if(pthread_sigmask(SIG_BLOCK, &mask, NULL) != 0)
      {
            perror("pthread_sigmask");
            exit(EXIT_FAILURE);
      }

      // ignoro SIGPIPE per evitare di essere terminato da una scrittura su un socket
      // va fatto sempre con i server che operano su socket
      struct sigaction s;
      memset(&s, 0, sizeof(s));    
      s.sa_handler = SIG_IGN;
      int r;
      SYSCALL_EXIT(sigaction, r, sigaction(SIGPIPE, &s, NULL), "sigaction", "");

      pthread_t sighandler_thread;
      safe_pthread_create(&sighandler_thread, NULL, sighandler_fun, &mask);
      
      //----------------------------------------//

      SYSCALL_EXIT(socket, listen_fd, socket(AF_UNIX, SOCK_STREAM, 0), "socket", "");

      struct sockaddr_un serv_addr;
      memset(&serv_addr, '0', sizeof(serv_addr));
      serv_addr.sun_family = AF_UNIX;    
      strncpy(serv_addr.sun_path, SOCKNAME, strlen(SOCKNAME) + 1);

      SYSCALL_EXIT(bind, r, bind(listen_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)), "bind", "");
      SYSCALL_EXIT(listen, r, listen(listen_fd, MAXBACKLOG), "listen", "");

      long conn_fd;
      while(end == 0)
      {     
            if((conn_fd = accept(listen_fd, (struct sockaddr *)NULL, NULL)) == -1)
            {
                  if(errno == EINVAL && end == 1)
                        break;
                  perror("accept");
                  exit(EXIT_FAILURE);
            }
            spawn_thread(conn_fd);
      }

      // aspetto la terminazione del sighandler_thread
      if(pthread_join(sighandler_thread, NULL) != 0)
            perror("pthread_join");
      
      unlink(SOCKNAME); // cleanup
      return 0;
}
