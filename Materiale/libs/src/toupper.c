#include <ctype.h>
#include <stddef.h>

void strtoupper(const char* in, size_t len, char* out) {
  for(int i=0;i<len;++i) // converto carattere per carattere
    out[i]=toupper(in[i]);
  out[len]='\0';
}

