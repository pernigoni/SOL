#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define N 1000
#define K1 100
#define K2 120
/*
      Piccolo bug: a causa della isNumber, se k1 oppure k2 valgono -1
      il programma dice che non è un numero.
*/
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
      int n, k1, k2;

      if(argc == 1)
            n = N, k1 = K1, k2 = K2;
      else if(argc == 4)
      {
            n = isNumber(argv[1]), k1 = isNumber(argv[2]), k2 = isNumber(argv[3]);
            
            if(n <= -1)
            {
                  printf("%s non è un numero oppure è negativo.\n", argv[1]);
                  return -1;
            }
            if(k1 == -1)
            {
                  printf("%s non è un numero.\n", argv[2]);
                  return -1;
            }
            if(k2 == -1)
            {
                  printf("%s non è un numero.\n", argv[3]);
                  return -1;
            }
            if(k1 >= k2)
            {
                  printf("K2 = %d deve essere maggiore di K1 = %d.\n", k2, k1);
                  return -1;
            }
      }
      else
      {
            printf("Usa: %s [K1 K2 N]\n", argv[0]);
            return -1;
      }

      int *occurrences = calloc(k2 - k1, sizeof(int));

      unsigned int seed = time(NULL);

      for(int i = 0; i < n; i++)
            occurrences[rand_r(&seed) % (k2 - k1)] += 1;
      
      printf("Occorrenze di:\n");
      for(int i = 0; i < k2 - k1; i++)
            printf("%-8d:%8.2f%%\n", i + k1, (occurrences[i] * 100.0) / n);

      free(occurrences);
      return 0;
}