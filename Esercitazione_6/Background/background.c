#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "../Utils/utils.h"

int main(int argc, char *argv[])
{
      if(argc != 2)
      {
            fprintf(stderr, "Usa: %s numero\n", argv[0]);
            exit(EXIT_FAILURE);
      }

      int x = isNumber(argv[1]);
      if(x <= 0)
      {
            fprintf(stderr, "%s non è un intero positivo\n", argv[1]);
            exit(EXIT_FAILURE);
      }

      int pid = fork();
      if(pid == 0) // primo figlio
      {
            if((pid = fork()) == 0) // secondo figlio
            {
                  execl("/bin/sleep", "/bin/sleep", x, (char *)NULL);
                  perror("execl");
                  exit(EXIT_FAILURE);
            }
            if(waitpid(pid, NULL, 0) == -1)
            {
                  perror("waitpid");
                  exit(EXIT_FAILURE);
            }
            printf("PID = %d, PPID = %d\n", getpid(), getppid());
      }
      return 0;
}