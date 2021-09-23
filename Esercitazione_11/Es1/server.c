/*
      Modello "Master-Workers", dove però il generico thread worker gestisce 
      interamente tutte le richieste di un client connesso.
*/
#include <ctype.h>
#include <signal.h>
#include <sys/select.h>
#include "../utils/utils.h"
#include "../conn/conn.h"
#include "../threadpool/threadpool.h"

typedef struct msg // messaggio
{
	int len;
	char *str;
} msg_t;

// struttura contenente le informazioni da passare al signal handler thread
typedef struct 
{
      sigset_t *set; // set dei segnali da gestire (mascherati)
      int signal_pipe; // descrittore di scrittura di una pipe senza nome
} sigHandler_t;

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

void* sighandler_fun(void *arg) 
{
      assert(arg);
      sigset_t *set = ((sigHandler_t *)arg)->set; // mask
      int fd_pipe = ((sigHandler_t *)arg)->signal_pipe;

      while(1)
      {
            int   sig,
                  r = sigwait(set, &sig);
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
                        printf("\nRicevuto %s, esco\n", (sig == SIGINT) ? "SIGINT" : ((sig == SIGTERM) ? "SIGTERM" : "SIGQUIT"));
                        // notifico il listener thread della ricezione del segnale
                        SYSCALL_EXIT(close, r, close(fd_pipe), "close", "");
                        return NULL;
                  default: ; 
            }
      }
      return NULL;
}

// funzione eseguita dal worker thread del pool, gestisce un'intera connessione di un client
void thread_fun(void *arg)
{
      assert(arg);
      long  *args = (long *)arg,
            conn_fd = args[0], *end = (long *)(args[1]);
      free(arg); // !!!!!

      fd_set set, tmp_set;
      FD_ZERO(&set);
      FD_SET(conn_fd, &set);

      do
      {
            tmp_set = set;

            // ogni tanto controllo se devo terminare
            struct timeval timeout = {0, 100000}; // 100 millisecondi

            int r;
            SYSCALL_PRINT(select, r, select(conn_fd + 1, &tmp_set, NULL, NULL, &timeout), "select", "");
            if(r == -1)
                  break;
            if(r == 0)
            {
                  if(*end == 1)
                        break;
                  continue;
            }

            msg_t msg;
            int n;
            SYSCALL_PRINT(readn, n, readn(conn_fd, &msg.len, sizeof(int)), "readn.1", "");
            if(n == -1 || n == 0)
                  break;
            msg.str = safe_calloc((msg.len), sizeof(char));
            SYSCALL_PRINT(readn, n, readn(conn_fd, msg.str, msg.len * sizeof(char)), "readn.2", "");
            if(n == -1)
            {
                  free(msg.str);
                  break;
            }

            toup(msg.str);

            SYSCALL_PRINT(writen, n, writen(conn_fd, &msg.len, sizeof(int)), "writen.1", "");
            if(n == -1)
            {
                  free(msg.str);
                  break;
            }
            SYSCALL_PRINT(writen, n, writen(conn_fd, msg.str, msg.len * sizeof(char)), "writen.2", "");
            if(n == -1)
            {
                  free(msg.str);
                  break;
            }
      
            free(msg.str);
      } while(*end == 0);

      int r;
      SYSCALL_EXIT(close, r, close(conn_fd), "close", "");
}

int main(int argc, char *argv[]) 
{
      cleanup();
      atexit(cleanup);

      if(argc != 2)
      {
            fprintf(stderr, "Usa: %s numero\n", argv[0]);
            exit(EXIT_FAILURE);
      }
      int n_threads_in_pool = isNumber(argv[1]);
      if(n_threads_in_pool <= 0)
      {
            fprintf(stderr, "%s non è un intero positivo\n", argv[1]);
            exit(EXIT_FAILURE);
      }

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

      /*
            La pipe viene utiliazzata come canale di comunicazione tra il signal handler thread
            e il thread listener per notificare la terminazione.
      */
      int signal_pipe[2];
      SYSCALL_EXIT(pipe, r, pipe(signal_pipe), "pipe", "");
      
      pthread_t sighandler_thread;
      sigHandler_t handler_args = {&mask, signal_pipe[1]};
      safe_pthread_create(&sighandler_thread, NULL, sighandler_fun, &handler_args);

      //----------------------------------------//
    
      int listen_fd;
      SYSCALL_EXIT(socket, listen_fd, socket(AF_UNIX, SOCK_STREAM, 0), "socket", "");

      struct sockaddr_un serv_addr;
      memset(&serv_addr, '0', sizeof(serv_addr));
      serv_addr.sun_family = AF_UNIX;    
      strncpy(serv_addr.sun_path, SOCKNAME, strlen(SOCKNAME) + 1);

      SYSCALL_EXIT(bind, r, bind(listen_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)), "bind", "");
      SYSCALL_EXIT(listen, r, listen(listen_fd, MAXBACKLOG), "listen", "");

      //----------------------------------------//

      threadpool_t *pool = createThreadPool(n_threads_in_pool, n_threads_in_pool);
      if(pool == NULL)
      {
            fprintf(stderr, "ERRORE: creando il thread pool\n");
            exit(EXIT_FAILURE);
      }

      fd_set set, tmp_set;
      // azzero sia il master set che il set temporaneo usato per la select
      FD_ZERO(&set);
      FD_ZERO(&tmp_set);
      // aggiungo listen_fd al master set
      FD_SET(listen_fd, &set);
      // aggiungo il descrittore di lettura della signal_pipe al master set
      FD_SET(signal_pipe[0], &set);
      // tengo traccia del file descriptor con id massimo
      int fd_max = (listen_fd > signal_pipe[0]) ? listen_fd : signal_pipe[0];

      volatile long end = 0;
      while(end == 0)
      {     
            // secondo la politica della select ogni volta devo ri-settare tmp_set (che è quello che gli passo) 
            tmp_set = set;
            SYSCALL_EXIT(select, r, select(fd_max + 1, &tmp_set, NULL, NULL, NULL), "select", ""); // + 1 FONDAMENTALE

            // cerchiamo di capire da quale fd abbiamo ricevuto una richiesta
            for(int i = 0; i <= fd_max; i++)
            {
                  if(FD_ISSET(i, &tmp_set))
                  {
                        long conn_fd;
                        if(i == listen_fd) // è una nuova richiesta di connessione di un nuovo client
                        {
                              SYSCALL_EXIT(accept, conn_fd, accept(listen_fd, (struct sockaddr *)NULL, NULL), "accept", "");

                              long *args = safe_malloc(2 * sizeof(long)); // faccio la free in thread_fun
                              args[0] = conn_fd;
                              args[1] = (long)&end;

                              r = addToThreadPool(pool, thread_fun, (void *)args);
                              if(r == 0) // aggiunto con successo
                                    continue;
                              else if(r == -1) // errore interno
                                    fprintf(stderr, "ERRORE: aggiungendo al thread pool\n");
                              else // coda dei pendenti piena
                                    fprintf(stderr, "SERVER MOLTO IMPEGNATO\n");

                              free(args);
                              SYSCALL_EXIT(close, r, close(conn_fd), "close", "");
                              continue;
                        }

                        if(i == signal_pipe[0]) 
                        {     // ricevuto un segnale, esco e inizio il protocollo di terminazione
                              end = 1;
                              break;
                        }
                  }
            }
      }
      // notifico che i thread dovranno uscire
      destroyThreadPool(pool, 0);  
      // aspetto la terminazione del signal handler thread
      if(pthread_join(sighandler_thread, NULL) != 0)
      {
            perror("pthread_join");
            exit(EXIT_FAILURE);
      }
      return 0;
}