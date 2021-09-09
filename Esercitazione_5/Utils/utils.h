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
      error_message: messaggio di errore.
*/
#define SYSCALL(name, value, system_call, error_msg, ...)    	\
      if((value = system_call) == -1)                             \
      {                                                           \
            perror(#name);                                        \
            int aux_errno = errno;                                \
            print_errors(error_msg, __VA_ARGS__);                 \
            exit(aux_errno);                                      \
      }

void print_errors(char *msg, ...);

long isNumber(const char *s);

#endif