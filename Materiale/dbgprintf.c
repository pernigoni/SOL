#include <stdio.h>
#include <stdarg.h>
/*
      #define DEBUG 

            oppure

      gcc -DDEBUG=1 -pedantic -Wall dbgprintf.c
*/
#if defined(DEBUG)
#define dbgprintf(...) \
      dbgprintf_impl(__FILE__, __LINE__, __VA_ARGS__);
// funzione di utilità per stampe di debugging
int dbgprintf_impl(const char *file, int line, const char *fmt, ...)
{
      va_list argp;
      va_start(argp, fmt);
      fprintf(stderr, ">>>>> [%s:%d]\n", file, line);
      vfprintf(stderr, fmt, argp);
      fprintf(stderr, "<<<<<\n");
      va_end(argp);
      return 0;
}
#else
#define dbgprintf(...)
#endif

int main()
{
      dbgprintf("ciao mondo\n");
      dbgprintf("questa è un'altra stampa in un altro punto %s %s %d\n", "ciao1", "ciao2", 3);
      return 0;
}