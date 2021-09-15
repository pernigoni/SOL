#include "../Utils/utils.h"
#include "../BQueue/boundedqueue.h"
#define Q1_SIZE 100
#define Q2_SIZE 1000
#define EOS (void *)0x1 // valore speciale usato per indicare End-Of-Stream

// read_line --> Q1 --> tok_on_space --> Q2 --> print_words

BQueue_t *Q1, *Q2; // sarebbe meglio passarle ai thread invece che dichiararle globali

void* read_line(void *file_name)
{
      FILE *ifp;
      FOPEN_ERR(ifp, (char *)file_name, "r");

      char *line = safe_malloc(512 * sizeof(char));
      while(fgets(line, 512, ifp) != NULL)
      {
            line[strlen(line) - 1] = '\0';
            if(strlen(line) == 0)
                  continue;
            push(Q1, line);

            line = safe_malloc(512 * sizeof(char));
      }

      free(line);
      fclose(ifp);
      push(Q1, EOS);
      return NULL;
}

void* tok_on_space()
{
      while(1)
      {
            char *line = pop(Q1);
            if(line == EOS)
                  break;

            char  *save = NULL,
                  *token = strtok_r(line, " ", &save);
            while(token)
            {
                  char *dup = strdup(token); // !!!!! strdup fa una malloc
                  push(Q2, dup);
                  token = strtok_r(NULL, " ", &save);
            }

            free(line); // !!!!! libero la memoria allocata dentro al while di read_line
      }
      push(Q2, EOS);
      return NULL;
}

void* print_words()
{
      while(1)
      {     
            char *word = pop(Q2);
            if(word == EOS)
                  break;
            if(word != NULL)
                  printf("%s\n", word);
            free(word); // !!!!! libero la memoria allocata con strdup in tok_on_space
      }
      return NULL;
}

int main(int argc, char *argv[])
{
      if(argc != 2)
      {
            fprintf(stderr, "Usa: %s nome-file\n", argv[0]);
            exit(EXIT_FAILURE);
      }

      Q1 = initBQueue(Q1_SIZE);
      Q2 = initBQueue(Q2_SIZE);

      pthread_t first, second, third;
      safe_pthread_create(&first, NULL, &read_line, argv[1]);
      safe_pthread_create(&second, NULL, &tok_on_space, NULL);
      safe_pthread_create(&third, NULL, &print_words, NULL);
      int status_1, status_2, status_3;
      if(pthread_join(first, (void *)&status_1) != 0)
            goto error_join;
      if(pthread_join(second, (void *)&status_2) != 0)
            goto error_join;
      if(pthread_join(third, (void *)&status_3) != 0)
            goto error_join;

      deleteBQueue(Q1, NULL);
      deleteBQueue(Q2, NULL);
      printf("------------------------------------\nPrimo thread terminato con stato   %d\nSecondo thread terminato con stato %d\nTerzo thread terminato con stato   %d\n", status_1, status_2, status_3);
      return 0;

error_join:
      perror("pthread_join");
      exit(EXIT_FAILURE);
}