#include <stdio.h>
#include <stdlib.h>

long isNumber(const char *s) 
{
      char *e = NULL;
      long val = strtol(s, &e, 0);
      if(e != NULL && *e == (char)0) 
            return val; 
      return -1;
}

int main()
{
      char *str = malloc(101 * sizeof(char));
      scanf("%s", str);
      printf("%d\n", (int)isNumber(str));
      free(str);
      return 0;
}