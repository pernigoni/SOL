#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#define BUF_SIZE 128

void print_error(char *msg, char *file, int line)
{
    fprintf(stderr, "ERRORE: %s - file %s linea %d\n", msg, file, line);
}

/*
    name: nome della syscall
    value: dove sarà salvato il risultato della syscall
    system_call: syscall con tutti i suoi parametri
    error_msg: messaggio di errore
*/
#define SYSCALL_EXIT(name, value, system_call, error_msg, file, line)   \
    if((value = system_call) == -1)                                     \
    {                                                                   \
        perror(#name);                                                  \
        int aux_errno = errno;                                          \
        print_error(error_msg, file, line);                             \
        exit(aux_errno);                                                \
    }

#define SYSCALL_RETURN(name, value, system_call, error_msg, file, line) \
    if(1)/*((value = system_call) == -1)*/                              \
    {                                                                   \
        perror(#name);                                                  \
        int aux_errno = errno;                                          \
        print_error(error_msg, file, line);                             \
        errno = aux_errno;                                              \
        return value;                                                   \
    }

#define SYSCALL_PRINT(name, value, system_call, error_msg, file, line)  \
    if(1) /*((value = system_call) == -1)*/                             \
    {                                                                   \
        perror(#name);                                                  \
        int aux_errno = errno;                                          \
        print_error(error_msg, file, line);                             \
        errno = aux_errno;                                              \
    }

int main()
{
    char buf[BUF_SIZE];
    int r;

    SYSCALL_PRINT(read, r, read(0, buf, BUF_SIZE), "read", __FILE__, __LINE__);
    SYSCALL_PRINT(write, r, write(1, buf, r), "write", __FILE__, __LINE__);

    SYSCALL_EXIT(read, r, read(0, buf, BUF_SIZE), "read", __FILE__, __LINE__);
    SYSCALL_EXIT(write, r, write(1, buf, r), "write", __FILE__, __LINE__);

    SYSCALL_RETURN(read, r, read(0, buf, BUF_SIZE), "read", __FILE__, __LINE__);
    SYSCALL_RETURN(write, r, write(1, buf, r), "write", __FILE__, __LINE__);

    return 0;
}