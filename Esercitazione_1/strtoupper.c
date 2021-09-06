#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

void strtoupper(const char *in, size_t len, char *out)
{
      for(int i = 0; i < len; i++)
            out[i] = toupper(in[i]);

      out[len] = '\0';
}

int main(int argc, char *argv[])
{
      if(argc == 1)
      {
            printf("Occorrono almeno 2 argomenti\n");
            return -1;
      }

      for(int i = 1; i < argc; i++)
      {
            char *out = malloc(strlen(argv[i]) * sizeof(char) + 1);

            if(out != NULL)
            {
                  strtoupper(argv[i], strlen(argv[i]), out);
                  printf("%s -> %s\n", argv[i], out);
                  free(out);
            }
            else
            {
                  printf("Memoria esaurita\n");
                  return -1;
            }
      }

      return 0;
}