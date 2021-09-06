#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/*
      INPUT 
            nome-programma "A B" "c d e"
      OUTPUT
            A
            c
            d
            e
            B
            c
            d
            e
*/
int main(int argc, char *argv[]) 
{
      if(argc != 3) 
      {
            fprintf(stderr, "use: %s stringa1 stringa2\n", argv[0]);
	      return -1;
      }
      
      // strtok_r modifica il primo parametro

      char  *save1 = NULL,
            *save2 = NULL,
            *tmp = calloc(strlen(argv[2]) + 1, sizeof(char)),
            *token1 = strtok_r(argv[1], " ", &save1);

      while(token1)
      {     
            printf("%s\n", token1);
            tmp = strncpy(tmp, argv[2], strlen(argv[2]));
            char *token2 = strtok_r(tmp, " ", &save2);
            
            while(token2)
            {
                  printf("%s\n", token2);
                  token2 = strtok_r(NULL, " ", &save2);  
            }
            
            token1 = strtok_r(NULL, " ", &save1);
      }

      free(tmp);
      return 0;
}