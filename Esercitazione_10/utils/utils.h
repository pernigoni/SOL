#ifndef _UTILS_H
#define _UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <errno.h>
#include <unistd.h>
#define MAX_ERROR_MSG_SIZE 512

/*
      name: nome della syscall
      value: dove sarà salvato il risultato della syscall
      system_call: syscall con tutti i suoi parametri
      error_msg: messaggio di errore
*/
#define SYSCALL_EXIT(name, value, system_call, error_msg, ...)    \
      if((value = system_call) == -1)                             \
      {                                                           \
            perror(#name);                                        \
            int aux_errno = errno;                                \
            print_error(error_msg, __VA_ARGS__);                  \
            exit(aux_errno);                                      \
      }

#define SYSCALL_RETURN(name, value, system_call, error_msg, ...)  \
      if((value = system_call) == -1)                             \
      {                                                           \
            perror(#name);                                        \
            int aux_errno = errno;                                \
            print_error(error_msg, __VA_ARGS__);                  \
            errno = aux_errno;                                    \
            return value;                                         \
      }

#define SYSCALL_PRINT(name, value, system_call, error_msg, ...)   \
      if((value = system_call) == -1)                             \
      {                                                           \
            perror(#name);                                        \
            int aux_errno = errno;                                \
            print_error(error_msg, __VA_ARGS__);                  \
            errno = aux_errno;                                    \
      }

#define SAFE_FOPEN(file, path, mode)                                                                  \
      if((file = fopen(path, mode)) == NULL)                                                          \
      {                                                                                               \
            fprintf(stderr, "ERRORE: impossibile aprire il file %s in modalità \"%s\"\n", path, mode);\
            exit(EXIT_FAILURE);                                                                       \
      }

#define SAFE_FCLOSE(file)           \
      if(fclose(file) == EOF)       \
      {                             \
            perror("fclose");       \
            exit(EXIT_FAILURE);     \
      }

long isNumber(const char *s);

void* safe_malloc(size_t size);

void* safe_calloc(size_t nmemb, size_t size);

void* safe_realloc(void *ptr, size_t size);

void print_error(char *msg, ...);

#endif