// prototipo della funzione printf
int printf(const char *format, ...);

static int x = 7;

static int foo(int y) {
  return x++ + y;
}


void stampa(int x) {
  printf("stampa: %d\n", foo(x));  
}

