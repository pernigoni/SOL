// -----------------------------
// Esercizio 1 Assegnamento 1
// -----------------------------

#include <stdlib.h>
#include <string.h>
#include <stdio.h>


#include <strtoupper.h>

int main(int argc, char* argv[]) {

  for(int i=1; i<argc; ++i) {
    size_t len = strlen(argv[i]);
    char* str;
    if ((str = malloc(len+1)) != NULL) {
      strtoupper(argv[i], len, str);
      printf("%s --> %s\n", argv[i], str);
      free(str);
    } else {
      printf("FATAL ERROR, memory exhausted\n");
      return -1;
    }
  }

  return 0;
}
