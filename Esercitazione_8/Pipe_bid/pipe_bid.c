#include <sys/types.h>
#include <sys/wait.h>
#include "../Utils/utils.h"

void process_1(int input, int output, const char *arg)
{     // input = channel_2_1[0], output = channel_1_2[1]
      int r;
      SYSCALL_EXIT(dup2, r, dup2(input, 0), "dup2", ""); 
      SYSCALL_EXIT(dup2, r, dup2(output, 1), "dup2", "");

      SYSCALL_EXIT(close, r, close(input), "close", "");
      SYSCALL_EXIT(close, r, close(output), "close", "");
      
      execl("./decr", "decr", arg, NULL);
      perror("execl");
      exit(EXIT_FAILURE);
}

void process_2(int input, int output)
{     // input = channel_1_2[0], output = channel_2_1[1]
      int r;
      SYSCALL_EXIT(dup2, r, dup2(input, 0), "dup2", ""); 
      SYSCALL_EXIT(dup2, r, dup2(output, 1), "dup2", "");

      SYSCALL_EXIT(close, r, close(input), "close", "");
      SYSCALL_EXIT(close, r, close(output), "close", "");
      
      execl("./decr", "decr", NULL);
      perror("execl");
      exit(EXIT_FAILURE);
}

int main(int argc, char *argv[])
{
      if(argc != 2)
      {
            fprintf(stderr, "Usa: %s numero-messaggi\n", argv[0]);
            exit(EXIT_FAILURE);
      }
      if(isNumber(argv[1]) <= 0)
      {
            fprintf(stderr, "%s non è un intero positivo\n", argv[1]);
            exit(EXIT_FAILURE);
      }
      /*
            proc_1 ----channel_1_2---> proc_2
            proc_1 <---channel_2_1---- proc_2
      */
      int channel_1_2[2], channel_2_1[2], r;
      SYSCALL_RETURN(pipe, r, pipe(channel_1_2), "pipe", "");
      SYSCALL_RETURN(pipe, r, pipe(channel_2_1), "pipe", "");

      if(fork() == 0) // primo figlio
      {
            SYSCALL_RETURN(close, r, close(channel_1_2[0]), "close", "");
            SYSCALL_RETURN(close, r, close(channel_2_1[1]), "close", "");
            process_1(channel_2_1[0], channel_1_2[1], argv[1]);
      }
      else // padre
      {
            if(fork() == 0) // secondo figlio
            {
                  SYSCALL_RETURN(close, r, close(channel_1_2[1]), "close", "");
                  SYSCALL_RETURN(close, r, close(channel_2_1[0]), "close", "");
                  process_2(channel_1_2[0], channel_2_1[1]);
            }
      }
      // padre (non usa la pipe quindi chiudo tutto)
      SYSCALL_RETURN(close, r, close(channel_1_2[0]), "close", "");
      SYSCALL_RETURN(close, r, close(channel_1_2[1]), "close", "");
      SYSCALL_RETURN(close, r, close(channel_2_1[0]), "close", "");
      SYSCALL_RETURN(close, r, close(channel_2_1[1]), "close", "");
      // aspetto la terminazione dei figli
      SYSCALL_RETURN(wait, r, wait(NULL), "wait", "");
      SYSCALL_RETURN(wait, r, wait(NULL), "wait", "");
      return 0;
}