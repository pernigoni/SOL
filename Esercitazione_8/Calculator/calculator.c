#include <sys/types.h>
#include <sys/wait.h>
#include "../Utils/utils.h"
#define MAX_LINE_LEN 256

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
      SYSCALL_RETURN(read, n, read(from_bc[0], result, MAX_LINE_LEN), "read", "");
      // chiudo stdin di bc così da farlo terminare
      SYSCALL_RETURN(close, r, close(to_bc[1]), "close", "");
      SYSCALL_RETURN(wait, r, wait(NULL), "wait", "");
      return n;
}

int main()
{
      while(1)
      {
            char line[MAX_LINE_LEN], result[MAX_LINE_LEN];
            if(fgets(line, MAX_LINE_LEN, stdin) == NULL)
            {
                  perror("fgets");
                  return -1;
            }
            if(strncmp(line, "exit", 4) == 0 || strncmp(line, "quit", 4) == 0)
                  break;

            int n;
            if((n = cmd(line, result)) == -1)
            {
                  fprintf(stderr, "ERRORE: nell'esecuzione del comando\n");
                  break;
            }
            result[n] = '\0';
            
            printf("Operazione: %sRisultato: %s\n", line, result);
      }
      return 0;
}