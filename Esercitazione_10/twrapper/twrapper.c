#include "twrapper.h"

void safe_pthread_create(pthread_t *tid, const pthread_attr_t *attr, void *(*start_routine) (void *), void *arg)
{
      if(pthread_create(tid, attr, *start_routine, arg) != 0)
      {
            perror("pthread_create");
            exit(EXIT_FAILURE);
      }
}

void safe_pthread_mutex_lock(pthread_mutex_t *mtx)
{
      if(pthread_mutex_lock(mtx) != 0)
      {
            perror("pthread_mutex_lock");
            exit(EXIT_FAILURE);
      }
}

void safe_pthread_mutex_unlock(pthread_mutex_t *mtx)
{
      if(pthread_mutex_unlock(mtx) != 0)
      {
            perror("pthread_mutex_unlock");
            exit(EXIT_FAILURE);
      }
}

void safe_pthread_cond_wait(pthread_cond_t *cond, pthread_mutex_t *mtx)
{
      if(pthread_cond_wait(cond, mtx) != 0)
      {
            perror("pthread_cond_wait");
            exit(EXIT_FAILURE);
      }
}

void safe_pthread_cond_signal(pthread_cond_t* cond)
{
      if(pthread_cond_signal(cond) != 0)
      {
            perror("pthread_cond_signal");
            exit(EXIT_FAILURE);
      }
}