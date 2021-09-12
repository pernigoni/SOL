#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>

// gcc -g -pedantic -Wall esempio_thread.c -lpthread

static int x, done;

static pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;

void safe_pthread_mutex_lock(pthread_mutex_t *mtx)
{
      int err;
      if((err = pthread_mutex_lock(mtx)) != 0)
      {
            errno = err;
            perror("pthread_mutex_lock");
            exit(EXIT_FAILURE);
      }
}

void safe_pthread_mutex_unlock(pthread_mutex_t *mtx)
{
      int err;
      if((err = pthread_mutex_unlock(mtx)) != 0)
      {
            errno = err;
            perror("pthread_mutex_unlock");
            exit(EXIT_FAILURE);
      }
}

static void* myfun(void *arg)
{
      while(1)
      {
            safe_pthread_mutex_lock(&mtx);
            
            done = (x >= 10);
            if(!done)
                  printf("Secondo thread: x = %d\n", ++x);

            safe_pthread_mutex_unlock(&mtx);

            if(done)
                  break;
            sleep(1);
      }

      /* Altro modo per fare la stessa cosa di sopra, però con questo è più facile sbagliare.
      safe_pthread_mutex_lock(&mtx);

      while(x < 10)
      {         
            printf("Secondo thread: x = %d\n", ++x);

            safe_pthread_mutex_unlock(&mtx);

            sleep(1);

            safe_pthread_mutex_lock(&mtx);
      }
      safe_pthread_mutex_unlock(&mtx);*/
}

int main()
{
      pthread_t tid;
      int err;
      if((err = pthread_create(&tid, NULL, &myfun, NULL)) != 0)
      {
            perror("pthread_create");
            exit(EXIT_FAILURE);
      }
      // secondo thread creato
      while(1) // non possiamo assumere un ordinamento tra le stampe
      {    
            safe_pthread_mutex_lock(&mtx);

            printf("Primo thread: x = %d\n", ++x);

            safe_pthread_mutex_unlock(&mtx);

            sleep(1);
      }
}