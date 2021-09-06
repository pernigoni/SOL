#include <stdio.h>
#include <stdlib.h>

#ifndef INIT_VALUE
#define INIT_VALUE 0
#endif

long isNumber(const char *s) 
{
      char *e = NULL;
      long val = strtol(s, &e, 0);
      if(e != NULL && *e == (char)0) 
            return val; 
      return -1;
}

void sum_r(int x, int *sum)
{
      *sum += x;
}

int main(int argc, char *argv[])
{
      if(argc != 2)
      {
            fprintf(stderr, "Usa: %s [N]\n", argv[0]);
            return -1;
      }

      int n = isNumber(argv[1]);
      if(n < 0)
      {
            fprintf(stderr, "%s è negativo oppure non è un numero.\n", argv[1]);
            return -1;
      }

      int x, sum = INIT_VALUE;
      for(int i = 0; i < n; i++)
      {
            scanf("%d", &x);
            sum_r(x, &sum);
      }
      printf("%d\n", sum);

      return 0;
}
