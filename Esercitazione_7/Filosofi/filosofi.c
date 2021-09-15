#include <stdatomic.h>
#include "../Utils/utils.h"
#define N 9 // numero totale di filosofi

typedef struct _thread_args_t
{
      int tid; // id del filosofo
      int n; // numero totale di filosofi
      pthread_mutex_t *forchette; // array di forchette
} thread_args_t;

static long ITER = 2000; // durata della cena

static inline long atomic_decrease(long *v)
{
      long r;
      r = atomic_fetch_sub(v, 1); // decrementa v di 1 in modo atomico, in alternativa si può fare con lock e unlock
      return r;
}

void eat(unsigned int *seed)
{
      long r = rand_r(seed) & 800000;
      struct timespec t = {0, r};
      nanosleep(&t, NULL);
}

void think(unsigned int *seed)
{
      long r = rand_r(seed) & 1000000;
      struct timespec t = {0, r};
      nanosleep(&t, NULL);
}

void *philosopher(void *arg)
{
      int   my_id = ((thread_args_t *)arg)->tid,
            n = ((thread_args_t *)arg)->n,
            left = my_id % n, // la forchetta sx è quella di indice corrente
            right = my_id - 1;

      pthread_mutex_t 
            *left_forchetta = &((thread_args_t *)arg)->forchette[left],
            *right_forchetta = &((thread_args_t *)arg)->forchette[right];
      
      unsigned int seed = my_id * time(NULL); // un seed diverso per ogni filosofo
      int eat_count = 0, think_count = 0;

      while(1)
      {
            if(atomic_decrease(&ITER) <= 0)
                  break;

            think(&seed);
            think_count++;

            /*    Evito la deadlock fissando un ordinamento totale tra i
                  filosofi per l'acquisizione delle forchette. 
            */
            if(my_id % 2) // il filosofo dispari prende prima la forchetta dx
            {
                  safe_pthread_mutex_lock(right_forchetta);
                  safe_pthread_mutex_lock(left_forchetta);
                  eat(&seed);
                  eat_count++;
                  safe_pthread_mutex_unlock(left_forchetta);
                  safe_pthread_mutex_unlock(right_forchetta);
            }
            else // il filosofo pari prende prima la forchetta sx
            {
                  safe_pthread_mutex_lock(left_forchetta);
                  safe_pthread_mutex_lock(right_forchetta);
                  eat(&seed);
                  eat_count++;
                  safe_pthread_mutex_unlock(right_forchetta);
                  safe_pthread_mutex_unlock(left_forchetta);
            }
      }

      printf("Filosofo %d : ho mangiato %d volte e pensato %d volte\n", my_id, eat_count, think_count);
      fflush(stdout);
      return NULL;
}

int main()
{
      pthread_t *tid_array = safe_malloc(N * sizeof(pthread_t));
      thread_args_t *args = safe_malloc(N * sizeof(thread_args_t));
      pthread_mutex_t *forchette = safe_malloc(N * sizeof(pthread_mutex_t)); // un mutex per ogni forchetta
      
      for(int i = 0; i < N; i++) // per l'inizializzazione devo usare init invece della macro
            if(pthread_mutex_init(&forchette[i], NULL) != 0)
            {
                  perror("pthread_mutex_init");
                  exit(EXIT_FAILURE);
            }
      for(int i = 0; i < N; i++)
      {
            args[i].tid = i + 1;
            args[i].n = N;
            args[i].forchette = forchette;
      }
      for(int i = 0; i < N; i++)
            safe_pthread_create(&tid_array[i], NULL, philosopher, &args[i]);
      for(int i = 0; i < N; i++)
            if(pthread_join(tid_array[i], NULL) != 0)
            {
                  perror("pthread_join");
                  exit(EXIT_FAILURE);
            }
            
      free(tid_array);
      free(args);
      free(forchette);
      return 0;
}