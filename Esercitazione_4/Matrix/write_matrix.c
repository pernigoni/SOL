#include <stdio.h>
#include <stdlib.h>

#define FOPEN_ERR(file, path, mode) \
      if((file = fopen(path, mode)) == NULL)                                                    \
      {                                                                                         \
            fprintf(stderr, "Impossibile aprire il file %s in modalità \"%s\".\n", path, mode);   \
            exit(-1);                                                                           \
      }

long isNumber(const char *s) 
{
      char *e = NULL;
      long val = strtol(s, &e, 0);
      if(e != NULL && *e == (char)0) 
            return val; 
      return -1;
}

int main(int argc, char *argv[])
{
      if(argc != 2)
      {
            fprintf(stderr, "Usa: %s numero\n", argv[0]);
            return -1;
      }
      
      int n = isNumber(argv[1]); // n = 30
      if(n < 0)
      {
            fprintf(stderr, "%s è negativo oppure non è un numero.\n", argv[1]);
            return -1;
      }
      else if(n > 512)
      {
            fprintf(stderr, "%s troppo grande (max 512).\n", argv[1]);
            return -1;
      }

      float mat[n][n];
      for(int i = 0; i < n; i++)
            for(int j = 0; j < n; j++)
                  mat[i][j] = (i + j) / 2.0;

      FILE *ofp_bin, *ofp_txt;
      FOPEN_ERR(ofp_bin, "./mat_dump.dat", "wb");
      FOPEN_ERR(ofp_txt, "./mat_dump.txt", "w");

      fwrite(mat, sizeof(float), n * n, ofp_bin);
      for(int i = 0; i < n; i++)
            for(int j = 0; j < n; j++)
                  fprintf(ofp_txt, "%f\n", mat[i][j]);

      fclose(ofp_bin);
      fclose(ofp_txt);
      return 0;
}