/*
      Modello "single-threaded".
      Il server fa uso della select per gestire sia nuove connessioni 
      da parte di nuovi client che la gestione dei messaggi inviati 
      dai client già connessi.
*/
#include <assert.h>
#include <ctype.h>
#include <sys/select.h>
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

int cmd(long conn_fd) 
{
      msg_t msg;
      
      int n;
      SYSCALL_RETURN(readn, n, readn(conn_fd, &msg.len, sizeof(int)), "readn.1", "");
      if(n == 0)
            return -1;
      msg.str = safe_calloc((msg.len), sizeof(char));
      SYSCALL_RETURN(readn, n, readn(conn_fd, msg.str, msg.len * sizeof(char)), "readn.2", "");

      toup(msg.str);

      SYSCALL_RETURN(writen, n, writen(conn_fd, &msg.len, sizeof(int)), "writen.1", "");
      SYSCALL_RETURN(writen, n, writen(conn_fd, msg.str, msg.len * sizeof(char)), "writen.2", "");
      free(msg.str);
      return 0;
}

// ritorno l'indice massimo tra i descrittori attivi
int updatemax(fd_set set, int fd_max) 
{
      for(int i = (fd_max - 1); i >= 0; i--)
            if(FD_ISSET(i, &set)) 
                  return i;
      return -1;
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
      
      fd_set set, tmp_set;
      // azzero sia il master set che il set temporaneo usato per la select
      FD_ZERO(&set);
      FD_ZERO(&tmp_set);

      // aggiungo listen_fd al master set
      FD_SET(listen_fd, &set);

      // tengo traccia del file descriptor con id massimo
      int fd_max = listen_fd, r; 

      while(1) 
      {      
            // secondo la politica della select ogni volta devo ri-settare tmp_set (che è quello che gli passo) 
            tmp_set = set;
                                          // + 1 FONDAMENTALE
            SYSCALL_EXIT(select, r, select(fd_max + 1, &tmp_set, NULL, NULL, NULL), "select", "");

            // cerchiamo di capire da quale fd abbiamo ricevuto una richiesta
            for(int i = 0; i <= fd_max; i++) 
            {
                  if(FD_ISSET(i, &tmp_set)) 
                  {
                        long conn_fd;

                        if(i == listen_fd) // è una nuova richiesta di connessione di un nuovo client
                        { 
                              SYSCALL_EXIT(accept, conn_fd, accept(listen_fd, (struct sockaddr *)NULL, NULL), "accept", "");
                              
                              // aggiungo il descrittore al master set
                              FD_SET(conn_fd, &set);

                              // ricalcolo il massimo
                              if(conn_fd > fd_max) 
                                    fd_max = conn_fd;
                              
                              continue;
                        }

                        // se arrivo qui è una nuova richiesta da un client già connesso
                        conn_fd = i;
                        
                        // eseguo il comando e se c'è un errore tolgo il descrittore dal master set
                        if(cmd(conn_fd) < 0) 
                        { 
                              SYSCALL_EXIT(close, r, close(conn_fd), "close", "");
                              printf("Connessione chiusa\n");

                              FD_CLR(conn_fd, &set); 
                              
                              // ricalcolo il massimo
                              if(conn_fd == fd_max) 
                                    fd_max = updatemax(set, fd_max);
                        }
                  }
            }
      }
      SYSCALL_EXIT(close, r, close(listen_fd), "close", "");
      return 0;
}