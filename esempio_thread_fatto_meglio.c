#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>

// gcc -g -pedantic -Wall esempio_thread_fatto_meglio.c -lpthread

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

static int incr_x(int incr) // una funzione con dentro un mutex difficilmente può essere resa rientrante
{
      static int x = 0;
      static pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;

      safe_pthread_mutex_lock(&mtx);
      int ret = (x += incr);
      safe_pthread_mutex_unlock(&mtx);

      return ret; // non devo fare return x; perché x è la variabile condivisa
}

static void* myfun(void *arg)
{
      while(1)
      {
            printf("Secondo thread: x = %d\n", incr_x(1));
            sleep(1);
      }
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
            printf("Primo thread: x = %d\n", incr_x(1));
            sleep(1);
      }
}