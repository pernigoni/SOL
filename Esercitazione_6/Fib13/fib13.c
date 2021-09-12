#include <unistd.h>
#include <sys/wait.h>
#include "../Utils/utils.h"
/* 
      Usiamo l'exit status per calcolare il numero di Fibonacci.
      L'exit status è codificato su 8 bit quindi il massimo numero di 
      Fibonacci calcolabile da 'doFib' è il tredicesimo.

      Calcola ricorsivamente il numero di Fibonacci dell'argomento 'n'.
      La soluzione deve effettuare fork di processi con il vincolo che 
      ogni processo esegua 'doFib' al più una volta.  
      Se l'argomento doPrint è 1 allora la funzione stampa il numero calcolato 
      prima di passarlo al processo padre.      
*/
static void doFib(int n, int doPrint)
{
      int   res = n,
            pid_1, pid_2;
      if(n >= 2)
      {
            if((pid_1 = fork()) == 0)
                  doFib(n - 1, 0);
            else
            {
                  if(pid_1 == -1)
                  {
                        perror("fork");
                        exit(EXIT_FAILURE);
                  }
                  if((pid_2 = fork()) == 0)
                        doFib(n - 2, 0);
                  else
                  {
                        if(pid_2 == -1)
                        {
                              perror("fork");
                              exit(EXIT_FAILURE);
                        }
                        int status;
                        if(waitpid(pid_1, &status, 0) == -1)
                        {
                              perror("waitpid");
                              exit(EXIT_FAILURE);
                        }

                        res = WEXITSTATUS(status);

                        if(waitpid(pid_2, &status, 0) == -1)
                        {
                              perror("waitpid");
                              exit(EXIT_FAILURE);
                        }

                        res += WEXITSTATUS(status);
                  }
            }
      }

      if(doPrint == 1)
            printf("fib(%d) = %d\n", n, res);

      exit(res);
}

int main(int argc, char *argv[])
{
      if(argc != 2)
      {
            fprintf(stderr, "Usa: %s numero\n", argv[0]);
            exit(EXIT_FAILURE);
      }

      int n = isNumber(argv[1]);
      if(n <= 0 || n > 14)
      {
            fprintf(stderr, "%s non è un intero positivo minore di 15\n", argv[1]);
            exit(EXIT_FAILURE);
      }

      doFib(n, 1);
      return 0;
}