#include <sys/types.h>
#include <unistd.h>
#include "../Utils/utils.h"
/*    
      Lanciare in background e visualizzare gli zombie con: 
            ps -A -ostat,pid,ppid | grep Z
*/
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

      for(int i = 0; i < n; i++)
            if(fork() == 0)
                  exit(EXIT_SUCCESS);
      
      printf("Il programma terminerà tra 20 secondi\n");
      sleep(20);
      return 0;
}