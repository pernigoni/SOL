#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "../Utils/utils.h"

void launch(int n)
{
      int ppid = getpid();
      if(n > 0)
      {
            for(int i = 0; i < n; i++)
                  printf("-");
            printf(" %d: creo un processo figlio\n", ppid);
            fflush(stdout);

            int pid = fork();
            if(pid == -1)
            {
                  perror("fork");
                  exit(EXIT_FAILURE);
            }
            if(pid == 0) // figlio
                  launch(n - 1);
            else // padre
            {
                  int status;
                  if(waitpid(pid, &status, 0) == -1)
                  {
                        perror("waitpid");
                        exit(EXIT_FAILURE);
                  }
                  if(!WIFEXITED(status))
                  {
                        printf("%d: figlio %d terminato con fallimento\n", ppid, pid);
                        fflush(stdout);
                  }
                  for(int i = 0; i < n - 1; i++)
                        printf("-");
                  if(n > 1)
                        printf(" ");
                  printf("%d: terminato con successo\n", pid);
                  fflush(stdout);
            }
      }
      else
            printf("%d: ultimo discendente\n", getpid());
}

int main(int argc, char *argv[])
{
      if(argc != 2)
      {
            fprintf(stderr, "Usa: %s numero\n", argv[0]);
            exit(EXIT_FAILURE);
      }

      int n = isNumber(argv[1]);
      if(n <= 0)
      {
            fprintf(stderr, "%s non è un intero positivo\n", argv[1]);
            exit(EXIT_FAILURE);
      }

      int pid = getpid();
      
      launch(n);

      if(pid == getpid())
      {
            for(int i = 0; i < n; i++)
                  printf("-");
            printf(" %d: terminato con successo\n", pid);
            fflush(stdout);
      }
      return 0;
}