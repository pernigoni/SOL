#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_LEN 101

#define FOPEN_ERR(file, path, mode) \
      if((file = fopen(path, mode)) == NULL)                                                    \
      {                                                                                         \
            fprintf(stderr, "Impossibile aprire il file %s in modalità \"%s\".", path, mode);   \
            exit(-1);                                                                           \
      }

int main(int argc, char *argv[])
{
      if(argc == 1)
            printf("Stampo sullo stdout:\n");
      else if(argc > 2)
      {
            fprintf(stderr, "Usa: %s [nome-file.txt]\n", argv[0]);
            return -1;
      }

      FILE *ifp, *ofp;

      FOPEN_ERR(ifp, "/etc/passwd", "r");

      if(argc == 1)
            ofp = stdout;
      else
            FOPEN_ERR(ofp, argv[1], "w");
      
      char *buf = malloc(MAX_LEN * sizeof(char));
      while(fgets(buf, MAX_LEN, ifp) != NULL)
      {
            char *s = strtok(buf, ":");
            fprintf(ofp, "%s\n", s);
      }

      fclose(ifp);
      if(argc == 2)
            fclose(ofp);
      free(buf);
      return 0;
}