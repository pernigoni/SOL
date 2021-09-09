// prototipo della funzione printf
int printf(const char *format, ...);

#include <stampa.h>

static int x = 3;

int foo(int x) {
  static int y = 5;
  
  return ++x + y++;
}

int main() {  
  
  printf("stampa1: %d\n", foo(3));
  printf("stampa2: %d\n", foo(7));
  printf("stampa3: %d\n", foo(-1));

  stampa(x);
  
  return 0;
}
