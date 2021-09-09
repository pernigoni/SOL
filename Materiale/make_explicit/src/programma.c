// -----------------------------
// Esercizio 1 Assegnamento 1
// -----------------------------

#include <stdlib.h>
#include <string.h>

#include <toupper.h>
#include <stampa.h>

int main(int argc, char* argv[]) {

  if(argc == 1) {
    errore("nessuna stringa in input");
    return 1;
  }

  for(int i=1; i<argc; ++i) {
    size_t len = strlen(argv[i]);
    char* str;
    if ((str = malloc(len+1)) != NULL) {
      strtoupper(argv[i], len, str);
      // printf("%s --> %s\n", argv[i], str);
      stampa(argv[i], str);
      free(str);
    } else {
      errore("FATAL ERROR, memory exhausted");
      return -1;
    }
  }

  return 0;
}
