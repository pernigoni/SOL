/*
	Il server spawna un processo che esegue bc.
	La comunicazione con il processo bc avviene attraverso 2 pipe.
	Il server risponde al client con la risposta ricevuta da bc.
*/
#include <sys/wait.h>
#include "../Utils/utils.h"
#include "../Conn/conn.h"
#define BUF_SIZE 256

typedef struct msg // messaggio
{
	int len;
	char *str;
} msg_t;

void cleanup() 
{
  	unlink(SOCKNAME);
}

// FUNZIONE COPIATA E INCOLLATA DA Esercitazione_8/Calculator/calculator.c
int cmd(char *input, char *result) // ritorna il numero di caratteri validi di result
{     /*
            padre -----to_bc---> bc
            padre <---from_bc--- bc
      */
      int   to_bc[2], // ci scrive il processo padre, ci legge bc
            from_bc[2]; // ci scrive bc, ci legge il processo padre

      int r;
      SYSCALL_RETURN(pipe, r, pipe(to_bc), "pipe", "");
      SYSCALL_RETURN(pipe, r, pipe(from_bc), "pipe", "");

      if(fork() == 0) // figlio
      {     // chiudo i descrittori che non servono
            SYSCALL_RETURN(close, r, close(to_bc[1]), "close", ""); 
            SYSCALL_RETURN(close, r, close(from_bc[0]), "close", "");
            // redirigo stdin sul descrittore con cui si legge dalla pipe
            SYSCALL_RETURN(dup2, r, dup2(to_bc[0], 0), "dup2", ""); 
            // redirigo stdout e stderr sul descrittore con cui si scrive sulla pipe
            SYSCALL_RETURN(dup2, r, dup2(from_bc[1], 1), "dup2", ""); 
            SYSCALL_RETURN(dup2, r, dup2(from_bc[1], 2), "dup2", "");

            execl("/usr/bin/bc", "bc", "-lq", NULL);
            perror("execl");
            return -1;
      }

      // padre
      // chiudo i descrittori che non servono
      SYSCALL_RETURN(close, r, close(to_bc[0]), "close", ""); 
      SYSCALL_RETURN(close, r, close(from_bc[1]), "close", "");
      
      int n;
      // scrivo il mio input sul descrittore con cui si scrive sulla pipe
      SYSCALL_RETURN(write, n, write(to_bc[1], input, strlen(input)), "write", "");
      // leggo il risultato o l'errore sul descrittore con cui si legge dalla pipe
      SYSCALL_RETURN(read, n, read(from_bc[0], result, BUF_SIZE), "read", "");
      // chiudo stdin di bc così da farlo terminare
      SYSCALL_RETURN(close, r, close(to_bc[1]), "close", "");
      SYSCALL_RETURN(wait, r, wait(NULL), "wait", "");
      return n;
}

int main(int argc, char *argv[]) 
{
	cleanup(); // cancello il socket file se esiste
	atexit(cleanup); // se qualcosa va storto

	// creo il socket
	int listen_fd;
	SYSCALL_EXIT(socket, listen_fd, socket(AF_UNIX, SOCK_STREAM, 0), "socket", ""); // sempre così

	// setto l'indirizzo 
	struct sockaddr_un serv_addr;
	memset(&serv_addr, '0', sizeof(serv_addr)); // azzero la struttura dati sockaddr
	serv_addr.sun_family = AF_UNIX; // specifico la famiglia
	strncpy(serv_addr.sun_path, SOCKNAME, strlen(SOCKNAME) + 1); // copio SOCKNAME dentro al nome del socket

	// assegno l'indirizzo al socket
	int not_used;
	SYSCALL_EXIT(bind, not_used, bind(listen_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)), "bind", "");
	
	// setto il socket in modalità passiva e definisco un numero massimo di connessioni pendenti (in questo caso 1)
	SYSCALL_EXIT(listen, not_used, listen(listen_fd, 1), "listen", "");

	int conn_fd, n;
	while(1) // il server termina con ctrl + C, con ls vedrò ancora il socket (non abbiamo ancora fatto i segnali)
	{	// dato che il server gestisce una connessione alla volta si mette in sttesa di fare una singola accept sul listen socket
		SYSCALL_EXIT(accept, conn_fd, accept(listen_fd, (struct sockaddr *)NULL, NULL), "accept", "");

		msg_t msg;
		msg.str = safe_malloc(BUF_SIZE * sizeof(char));

		while(1) 
		{     
			char buf[BUF_SIZE];
			memset(msg.str, '\0', BUF_SIZE);

			// leggo la size della stringa
			SYSCALL_EXIT(readn, n, readn(conn_fd, &msg.len, sizeof(int)), "readn.1", ""); 
			// leggo la stringa
			SYSCALL_EXIT(readn, n, readn(conn_fd, msg.str, msg.len), "readn.2", "");
			
			if(n == 0) 
				break;
			
			// il server ha ricevuto il messaggio, preparo il buffer per la risposta
			memset(buf, '\0', BUF_SIZE);
			if((n = cmd(msg.str, buf)) == -1) 
			{ 
				fprintf(stderr, "ERRORE: eseguendo il comando\n");
				break;
			}
			buf[n] = '\0';
			
			// invio la risposta al client (con lo stesso formato con cui ho letto il messaggio)
			SYSCALL_EXIT(writen, not_used, writen(conn_fd, &n, sizeof(int)), "writen.1", "");
			SYSCALL_EXIT(writen, not_used, writen(conn_fd, buf, n), "writen.2", "");	    
		}

		int r;
		SYSCALL_EXIT(close, r, close(conn_fd), "close", "");
		printf("Connessione chiusa\n");

		if(msg.str != NULL) 
			free(msg.str);
	}
	return 0;
} 