#include "utils.h"

long isNumber(const char *s) 
{
      char *e = NULL;
      long val = strtol(s, &e, 0);
      if(e != NULL && *e == (char)0) 
            return val; 
      return -1;
}

void* safe_malloc(size_t size)
{
      void *buf;
      if((buf = malloc(size)) == NULL)
      {
            perror("malloc");
            exit(EXIT_FAILURE);
      }
      return buf;
}

void* safe_calloc(size_t nmemb, size_t size)
{
      void *buf;
      if((buf = calloc(nmemb, size)) == NULL)
      {
            perror("calloc");
            exit(EXIT_FAILURE);
      }
      return buf;
}

void* safe_realloc(void *ptr, size_t size)
{
      void *buf;
      if((buf = realloc(ptr, size)) == NULL)
      {
            perror("realloc");
            exit(EXIT_FAILURE);
      }
      return buf;
}

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

void print_error(char *msg, ...)
{
	char 	*begin = "ERRORE: ",
		*err = safe_calloc(strlen(begin) + strlen(msg) + MAX_ERROR_MSG_SIZE, sizeof(char));
	
	strncat(err, begin, strlen(begin));
	strncat(err, msg, strlen(msg));

	va_list va;
	va_start(va, msg);
	vfprintf(stderr, err, va);
	va_end(va);
	free(err);
}