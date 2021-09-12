#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include "../Utils/utils.h"
#define MAX_ARGS 128
#define MAX_LINE_LEN 512

void type_prompt()
{
      printf("dummyshell> ");
      fflush(stdout);
}

void free_argv(int argc, char *argv[])
{
      for(int i = 0; i < argc; i++)
            free(argv[i]);
}

int read_cmd_line(int *argc, char *argv[], int max_arg)
{
      free_argv(*argc, argv);

      char line[MAX_LINE_LEN];
      if(fgets(line, MAX_LINE_LEN, stdin) == NULL)
      {
            perror("fgets");
            return -1;
      }
      
      char  *save = NULL, 
            *token = strtok_r(line, " ", &save);
      *argc = 0;
      while(token)
      {
            if(*argc > max_arg - 1)
            {
                  fprintf(stderr, "ERRORE: troppi argomenti\n");
                  return -1;
            }
            argv[*argc] = safe_calloc(strlen(token) + 1, sizeof(char));
            strncpy(argv[*argc], token, strlen(token));
            token = strtok_r(NULL, " ", &save);
            *argc += 1;
      }
      argv[*argc] = NULL;
      return 0;
}

int cmd_exit(char *argv[])
{
      if(strncmp(argv[0], "exit", 4) == 0)
            return 0;
      return -1;
}

static void execute(int argc, char *argv[])
{
      int pid, status;
      switch(pid = fork())
      {
            case -1: // errore
                  perror("fork");
                  exit(EXIT_FAILURE);
            case 0: // figlio
                  execvp(argv[0], argv);
                  perror("execvp"); // se arrivo qui vuol dire che c'è stato un errore nella exec
                  free_argv(argc, argv);
                  exit(EXIT_FAILURE);
            default: // padre
                  if((pid = waitpid(-1, &status, 0)) == -1)
                  {
                        perror("waitpid");
                        exit(EXIT_FAILURE);
                  }
                  else if(pid != 0)
                        if(WIFEXITED(status))
                        {
                              printf("Processo %d terminato con stato %d\n", pid, WEXITSTATUS(status));
                              fflush(stdout);
                        }
      }
}

int main()
{
      int argc = 0;
      char *argv[MAX_ARGS];

      while(1)
      {
            type_prompt();
      
            if(read_cmd_line(&argc, argv, MAX_ARGS) == 0)
            {
                  if(cmd_exit(argv) == 0)
                  {
                        free_argv(argc, argv);
                        exit(EXIT_SUCCESS);
                  }
                  /*printf("STAMPA: ");
                  for(int i = 0; i < argc; i++)
                        printf("argv[%d]->%s ", i, argv[i]);
                  printf("\n");*/
                  
                  execute(argc, argv);
            }
      }
      return 0;
}