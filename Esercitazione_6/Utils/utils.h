#ifndef _UTILS_H

#define _UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <errno.h>
#define MAX_ERROR_MSG_SIZE 1024

/*
      name: nome della syscall.
      value: dove sarà salvato il risultato della syscall.
      system_call: syscall con tutti i suoi parametri.
      error_msg: messaggio di errore.
*/
#define SYSCALL(name, value, system_call, error_msg, ...)    	\
      if((value = system_call) == -1)                             \
      {                                                           \
            perror(#name);                                        \
            int aux_errno = errno;                                \
            print_errors(error_msg, __VA_ARGS__);                 \
            exit(aux_errno);                                      \
      }

#define FOPEN_ERR(file, path, mode) \
      if((file = fopen(path, mode)) == NULL)                                                          \
      {                                                                                               \
            fprintf(stderr, "ERRORE: impossibile aprire il file %s in modalità \"%s\"\n", path, mode);\
            exit(-1);                                                                                 \
      }

void print_errors(char *msg, ...);

long isNumber(const char *s);

void* safe_malloc(size_t size);

void* safe_calloc(size_t nmemb, size_t size);

void* safe_realloc(void *ptr, size_t size);

#endif