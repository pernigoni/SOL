#include "threadpool.h"

/**
 *    @brief Funzione eseguita dal thread worker che appartiene al pool
 */
static void* workerpool_thread(void *threadpool) 
{    
      threadpool_t *pool = (threadpool_t *)threadpool; // cast
      taskfun_t task; // task generico
      pthread_t self = pthread_self();
      int myid = -1;

      do // non efficiente, si può fare meglio...
      {
            for(int i = 0; i < pool->numthreads; i++)
                  if(pthread_equal(pool->threads[i], self)) 
                  {
                        myid = i;
                        break;
                  }
      } while(myid < 0);

      safe_pthread_mutex_lock(&(pool->lock));
      while(1)
      {
            while((pool->count == 0) && (!pool->exiting)) // in attesa di un messaggio, controllo spurious wakeups
                  pthread_cond_wait(&(pool->cond), &(pool->lock));

            if(pool->exiting > 1) // exit forzato, esco immediatamente
                  break;
            
            // devo uscire ma ci sono messaggi pendenti 
            if(pool->exiting == 1 && !pool->count) 
                  break;  

            // nuovo task
            task.fun = pool->pending_queue[pool->head].fun;
            task.arg = pool->pending_queue[pool->head].arg;

            pool->head++; 
            pool->count--;
            pool->head = (pool->head == abs(pool->queue_size)) ? 0 : pool->head;

            pool->taskonthefly++;
            safe_pthread_mutex_unlock(&(pool->lock));

            // eseguo la funzione 
            (*(task.fun))(task.arg);
            
            safe_pthread_mutex_lock(&(pool->lock));
            pool->taskonthefly--;
      }
      safe_pthread_mutex_unlock(&(pool->lock));

      fprintf(stderr, "Thread %d uscito\n", myid);
      return NULL;
}

/**
 *    @brief Funzione eseguita dal thread worker che non appartiene al pool
 */
static void* proxy_thread(void *arg) 
{    
      taskfun_t *task = (taskfun_t *)arg;
      (*(task->fun))(task->arg); // eseguo la funzione 
      free(task);
      return NULL;
}

static int freePoolResources(threadpool_t *pool) 
{
      if(pool->threads) 
      {
            free(pool->threads);
            free(pool->pending_queue);
            pthread_mutex_destroy(&(pool->lock));
            pthread_cond_destroy(&(pool->cond));
      }
      free(pool);    
      return 0;
}

threadpool_t* createThreadPool(int numthreads, int pending_size) 
{
      if(numthreads <= 0 || pending_size < 0) 
      {
            errno = EINVAL;
            return NULL;
      }
      
      threadpool_t *pool = safe_malloc(sizeof(threadpool_t));

      // condizioni iniziali
      pool->numthreads = 0;
      pool->taskonthefly = 0;
      pool->queue_size = (pending_size == 0 ? -1 : pending_size);
      pool->head = pool->tail = pool->count = 0;
      pool->exiting = 0;

      // alloco thread e task queue
      pool->threads = safe_malloc(sizeof(pthread_t) * numthreads);
      pool->pending_queue = safe_malloc(sizeof(taskfun_t) * abs(pool->queue_size));
      
      if((pthread_mutex_init(&(pool->lock), NULL) != 0) || (pthread_cond_init(&(pool->cond), NULL) != 0))  
      {
            free(pool->threads);
            free(pool->pending_queue);
            free(pool);
            return NULL;
      }

      for(int i = 0; i < numthreads; i++) 
      {
            if(pthread_create(&(pool->threads[i]), NULL, workerpool_thread, (void *)pool) != 0) 
            {     // errore fatale, libero tutto forzando l'uscita dei threads
                  destroyThreadPool(pool, 1);
                  errno = EFAULT;
                  return NULL;
            }
            pool->numthreads++;
      }
      return pool;
}

int destroyThreadPool(threadpool_t *pool, int force) 
{    
      if(pool == NULL || force < 0) 
      {
            errno = EINVAL;
            return -1;
      }

      safe_pthread_mutex_lock(&(pool->lock));
      pool->exiting = 1 + force;
      if(pthread_cond_broadcast(&(pool->cond)) != 0) 
      {
            safe_pthread_mutex_unlock(&(pool->lock));
            errno = EFAULT;
            return -1;
      }
      safe_pthread_mutex_unlock(&(pool->lock));

      for(int i = 0; i < pool->numthreads; i++) 
      {
            if(pthread_join(pool->threads[i], NULL) != 0) 
            {
                  errno = EFAULT;
                  safe_pthread_mutex_unlock(&(pool->lock));
                  return -1;
            }
      }
      freePoolResources(pool);
      return 0;
}

int addToThreadPool(threadpool_t *pool, void (*f)(void *), void *arg) 
{
      if(pool == NULL || f == NULL) 
      {
            errno = EINVAL;
            return -1;
      }

      safe_pthread_mutex_lock(&(pool->lock));
      int queue_size = abs(pool->queue_size);
      int nopending  = (pool->queue_size == -1); // non dobbiamo gestire messaggi pendenti

      // coda piena o in fase di uscita
      if(pool->count >= queue_size || pool->exiting)  
      {
            safe_pthread_mutex_unlock(&(pool->lock));
            return 1; // esco con valore "coda piena"
      }
      
      if(pool->taskonthefly >= pool->numthreads) 
      {
            if(nopending) 
            {     // tutti i thread sono occupati e non si gestiscono task pendenti
                  assert(pool->count == 0);
                  safe_pthread_mutex_unlock(&(pool->lock));
                  return 1; // esco con valore "coda piena"
            }
      } 
      
      pool->pending_queue[pool->tail].fun = f;
      pool->pending_queue[pool->tail].arg = arg;
      pool->count++;    
      pool->tail++;
      if(pool->tail >= queue_size) 
            pool->tail = 0;
      
      int r;
      if((r = pthread_cond_signal(&(pool->cond))) != 0) 
      {
            safe_pthread_mutex_unlock(&(pool->lock));
            errno = r;
            return -1;
      }
      safe_pthread_mutex_unlock(&(pool->lock));
      return 0;
}

int spawnThread(void (*f)(void *), void *arg) 
{
      if (f == NULL) 
      {
            errno = EINVAL;
            return -1;
      }

      taskfun_t *task = safe_malloc(sizeof(taskfun_t)); // la memoria verrà liberata dal proxy 
      task->fun = f;
      task->arg = arg;

      pthread_t thread;
      pthread_attr_t attr;
      if(pthread_attr_init(&attr) != 0) 
            return -1;
      if(pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED) != 0) 
            return -1;
      if(pthread_create(&thread, &attr, proxy_thread, (void *)task) != 0) 
      {
            free(task);
            errno = EFAULT;
            return -1;
      }
      return 0;
}