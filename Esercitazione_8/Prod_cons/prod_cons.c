#include <assert.h>
#include "../Utils/utils.h"
#include "../UQueue/queue.h"
#define N_PROD 500 // numero di thread produttori
#define N_CONS 10 // numero di thread consumatori

typedef struct _thread_args_t
{
      int tid;
      Queue_t *Q;
      int start;
      int stop;
} thread_args_t;

void* producer(void *arg)
{
      Queue_t *Q = ((thread_args_t *)arg)->Q;
      int   my_id = ((thread_args_t *)arg)->tid,
            my_start = ((thread_args_t *)arg)->start,
            my_stop = ((thread_args_t *)arg)->stop;
      
      for(int i = my_start; i < my_stop; i++)
      {
            int *data = malloc(1 * sizeof(int));
            if(data == NULL)
            {
                  perror("malloc");
                  pthread_exit(NULL);
            }
            *data = i;
            if(push(Q, data) == -1)
            {
                  fprintf(stderr, "ERRORE: push\n");
                  pthread_exit(NULL);
            }
            printf("Producer %d pushed: %d\n", my_id, *data);
      }
      printf("Producer %d exits\n", my_id);
      return NULL;
}

void* consumer(void *arg)
{
      Queue_t *Q = ((thread_args_t *)arg)->Q;
      int   my_id = ((thread_args_t *)arg)->tid, consumed = 0;
      
      while(1)
      {
            int *data = pop(Q);
            assert(data != NULL);
            if(*data == -1)
            {
                  free(data);
                  break;
            }
            consumed++;
            printf("Consumer %d popped: %d\n", my_id, *data);
            free(data);
      }
      printf("Consumer %d, consumed %d messages, now it exits\n", my_id, consumed);
      return NULL;
}

int main(int argc, char *argv[])
{
      if(argc != 2)
      {
            fprintf(stderr, "Usa: %s numero-messaggi\n", argv[0]);
            exit(EXIT_FAILURE);
      }

      int k = isNumber(argv[1]);
      if(k <= 0)
      {
            fprintf(stderr, "%s non è un intero positivo\n", argv[1]);
            exit(EXIT_FAILURE);
      }

      pthread_t *tid_array = safe_malloc((N_PROD + N_CONS) * sizeof(pthread_t));
      thread_args_t *args = safe_malloc((N_PROD + N_CONS) * sizeof(thread_args_t));

      Queue_t *Q = initQueue();
      if(Q == NULL)
      {
            fprintf(stderr, "ERRORE: inizializzando la coda\n");
            exit(errno);
      }

      int chunk = k / N_PROD, r = k % N_PROD, start = 0;
      for(int i = 0; i < N_PROD; i++)
      {
            args[i].tid = i;
            args[i].Q = Q;
            args[i].start = start;
            args[i].stop = start + chunk + (i < r ? 1 : 0);
            start = args[i].stop;
      }
      for(int i = N_PROD; i < N_PROD + N_CONS; i++)
      {
            args[i].tid = i - N_PROD;
            args[i].Q = Q;
            args[i].start = -1;
            args[i].stop = -1;
      }
      for(int i = 0; i < N_PROD; i++)
            safe_pthread_create(&tid_array[i], NULL, producer, &args[i]);
      for(int i = N_PROD; i < N_PROD + N_CONS; i++)
            safe_pthread_create(&tid_array[i], NULL, consumer, &args[i]);

      /*
            POSSIBILE PROTOCOLLO DI TERMINAZIONE
            Si aspettano prima tutti i produttori, quindi si inviano N_CONS
            valori speciali (-1), quindi si aspettano i consumatori.
      */
      for(int i = 0; i < N_PROD; i++) // aspetto prima tutti i produttori
            if(pthread_join(tid_array[i], NULL) != 0)
                  goto error_join;
      for(int i = 0; i < N_CONS; i++) // invio N_CONS valori speciali
      {
            int *eos = safe_malloc(1 * sizeof(int));
            *eos = -1;
            push(Q, eos);
      }
      for(int i = N_PROD; i < N_PROD + N_CONS; i++) // aspetto tutti i consumatori
            if(pthread_join(tid_array[i], NULL) != 0)
                  goto error_join;

      printf("----\nFINE\n");
      deleteQueue(Q);
      free(tid_array);
      free(args);
      return 0;

error_join:
      perror("pthread_join");
      exit(EXIT_FAILURE);
}