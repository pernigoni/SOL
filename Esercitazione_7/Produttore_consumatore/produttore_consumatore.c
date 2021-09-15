#include "../Utils/utils.h"
#define LOOP 20
#define BUF_SIZE 1

// !!!!! UN ELEMENTO NON VIENE CONSUMATO COME SI DEVE

int   buf[BUF_SIZE] = {0}, // struttura dati condivisa
      n_elem = 0,
      end_production = 0;

static unsigned int seed;

pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t empty = PTHREAD_COND_INITIALIZER;
pthread_cond_t full = PTHREAD_COND_INITIALIZER;

int produce(unsigned int *seed)
{
      return (rand_r(seed) % 1000);
}

void* producer(void *arg)
{
      for(int i = 0; i < LOOP; i++)
      {
            safe_pthread_mutex_lock(&mtx);

            while(n_elem == BUF_SIZE) // pieno
                  safe_pthread_cond_wait(&full, &mtx);

            buf[0] = produce(&seed);
            n_elem++;

            safe_pthread_cond_signal(&empty);

            safe_pthread_mutex_unlock(&mtx);

            printf("Produttore  : %d\n", buf[0]);
            fflush(stdout);
            //sleep(1);
      }

      end_production = 1;
      return NULL;
}

void* consumer(void *arg)
{
      while(1)
      {
            if(end_production == 1)
                  break;

            safe_pthread_mutex_lock(&mtx);

            while(n_elem == 0) // vuoto
                  safe_pthread_cond_wait(&empty, &mtx);

            n_elem--;

            safe_pthread_cond_signal(&full); 

            safe_pthread_mutex_unlock(&mtx);
            
            printf("Consumatore : %d\n", buf[0]);
            fflush(stdout);
            //sleep(1);
      }
      return NULL;
}

int main()
{
      seed = time(NULL);

      pthread_t p, c;

      safe_pthread_create(&p, NULL, &producer, NULL);
      safe_pthread_create(&c, NULL, &consumer, NULL);
      
      int status_p, status_c;
      if(pthread_join(p, (void *)&status_p) != 0) // non sono riuscito a fare una safe_pthread_join
      {
            perror("pthread_join");
            exit(EXIT_FAILURE);
      }
      if(pthread_join(c, (void *)&status_c) != 0)
      {
            perror("pthread_join");
            exit(EXIT_FAILURE);  
      }

      printf("-----------------------------------------\nThread Produttore terminato con status  %d\nThread Consumatore terminato con status %d\n", status_p, status_c);
      return 0;
}