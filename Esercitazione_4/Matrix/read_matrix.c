#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../Utils/utils.h"

typedef int F_t(const void *s1, const void *s2, size_t len);

int my_compare(F_t fun, const void *s1, const void *s2, size_t len)
{
      return fun(s1, s2, len);  
}

int main(int argc, char *argv[])
{    
      if(argc != 4)
      {
            fprintf(stderr, "Usa: %s nome-file.dat nome-file.txt\n", argv[0]);
            return -1;
      }
      
      int n = isNumber(argv[3]); // n = 30
      if(n < 0)
      {
            fprintf(stderr, "%s è negativo oppure non è un numero.\n", argv[3]);
            return -1;
      }
      else if(n > 512)
      {
            fprintf(stderr, "%s troppo grande (max 512).\n", argv[3]);
            return -1;
      }

      float mat_bin[n][n], mat_txt[n][n];

      FILE *ifp_bin, *ifp_txt;
      FOPEN_ERR(ifp_bin, argv[1], "rb");
      FOPEN_ERR(ifp_txt, argv[2], "r");

      fread(mat_bin, sizeof(float), n * n, ifp_bin);
      for(int i = 0; i < n; i++)
            for(int j = 0; j < n; j++)
                  fscanf(ifp_txt, "%f", &mat_txt[i][j]);

      if(my_compare(memcmp, mat_bin, mat_txt, n * n) == 0)
            printf("Le matrici sono uguali.\n");
      else
            printf("Le matrici sono diverse.\n");

      fclose(ifp_bin);
      fclose(ifp_txt);
      return 0;
}