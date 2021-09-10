#ifndef _UTILS_H

#define _UTILS_H

#include <stdio.h>
#include <stdlib.h>

#define FOPEN_ERR(file, path, mode) \
      if((file = fopen(path, mode)) == NULL)                                                    \
      {                                                                                         \
            fprintf(stderr, "Impossibile aprire il file %s in modalità \"%s\"\n", path, mode);  \
            exit(-1);                                                                           \
      }

long isNumber(const char *s);

#endif
