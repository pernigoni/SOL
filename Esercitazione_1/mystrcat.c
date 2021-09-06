#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
/*
      Concatena le prime 6 stringhe.
*/

const int REALLOC_INC = 16;

/*
      Con la macro la chiamata è REALLOC(buf, sz);

#define REALLOC(buf, newsize)                   \
      buf = realloc(buf, newsize);              \
      if(buf == NULL)                           \
      {                                         \
            printf("Memoria esaurita\n");       \
            free(buf);                          \
            exit(-1);                           \
      }
*/

void REALLOC(char **buf, size_t newsize) 
{
      *buf = realloc(*buf, newsize);
      if(*buf == NULL)
      {
            printf("Memoria esaurita\n");
            free(*buf);
            exit(-1); // uscita brutale di tutto il processo (return -1; nel main) 
      }   
}

char* mystrcat(char *buf, size_t sz, char *first, ...) 
{
      va_list va;
      va_start(va, *first);

      char *curr = first;
      while(curr != NULL)
      {     
            if(sz - strlen(buf) < strlen(curr) + 1)
            {
                  sz += strlen(curr) + 1;
                  REALLOC(&buf, sz);
            }
            buf = strncat(buf, curr, strlen(curr) + 1);
            curr = va_arg(va, char *);

            // printf("%lu\n", strlen(buf));
      }

      va_end(va);
      return buf;
}

int main(int argc, char *argv[]) 
{
      if(argc < 7) 
      { 
            printf("Troppi pochi argomenti\n"); 
            return -1; 
      }
      char *buffer = NULL;
      REALLOC(&buffer, REALLOC_INC); //macro che effettua l'allocazione del buffer
      buffer[0] = '\0'; //mi assicuro che il buffer contenga una stringa
      buffer = mystrcat(buffer, REALLOC_INC, argv[1], argv[2], argv[3], argv[4], argv[5], argv[6], NULL);
      printf("%s\n", buffer);     
      free(buffer);
      return 0;
}