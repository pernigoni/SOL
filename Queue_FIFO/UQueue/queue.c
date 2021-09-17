#include <errno.h>
#include <assert.h>

#include "../Utils/utils.h"
#include "queue.h"

static inline Node_t* alloc_node()                  
{ 
      return malloc(sizeof(Node_t));
}

static inline Queue_t* alloc_queue()                 
{ 
      return malloc(sizeof(Queue_t)); 
}

static inline void free_node(Node_t *node)           
{ 
      free((void *)node); 
}

static inline void lock_queue(Queue_t *q)            
{ 
      safe_pthread_mutex_lock(&q->qlock);   
}

static inline void unlock_queue(Queue_t *q)          
{ 
      safe_pthread_mutex_unlock(&q->qlock); 
}

static inline void unlock_queue_and_wait(Queue_t *q)   
{ 
      safe_pthread_cond_wait(&q->qcond, &q->qlock); 
}

static inline void unlock_queue_and_signal(Queue_t *q) 
{ 
      safe_pthread_cond_signal(&q->qcond); 
      safe_pthread_mutex_unlock(&q->qlock); 
}

Queue_t* initQueue() 
{
      Queue_t *q = alloc_queue();
      if(!q) 
            return NULL;
      q->head = alloc_node();
      if(!q->head) 
            return NULL;
      q->head->data = NULL; 
      q->head->next = NULL;
      q->tail = q->head;    
      q->qlen = 0;
      if(pthread_mutex_init(&q->qlock, NULL) != 0) 
      {
            perror("pthread_mutex_init");
            return NULL;
      }
      if(pthread_cond_init(&q->qcond, NULL) != 0) 
      {
            perror("pthread_cond_init");
            if(&q->qlock) 
                  pthread_mutex_destroy(&q->qlock);
            return NULL;
      }    
      return q;
}

void deleteQueue(Queue_t *q) 
{
      while(q->head != q->tail) 
      {
            Node_t *p = (Node_t *)q->head;
            q->head = q->head->next;
            free_node(p);
      }
      if(q->head) 
            free_node((void*)q->head);
      if(&q->qlock)  
            pthread_mutex_destroy(&q->qlock);
      if(&q->qcond)  
            pthread_cond_destroy(&q->qcond);
      free(q);
}

int push(Queue_t *q, void *data) 
{
      if((q == NULL) || (data == NULL)) 
      { 
            errno = EINVAL; 
            return -1;
      }
      Node_t *n = alloc_node();
      if(!n) 
            return -1;
      n->data = data; 
      n->next = NULL;

      lock_queue(q);
      q->tail->next = n;
      q->tail = n;
      q->qlen += 1;
      unlock_queue_and_signal(q);

      return 0;
}

void* pop(Queue_t *q) 
{        
      if(q == NULL) 
      { 
            errno = EINVAL; 
            return NULL;
      }

      lock_queue(q);
      while(q->head == q->tail)
            unlock_queue_and_wait(q);
      // locked
      assert(q->head->next);
      Node_t *n = (Node_t *)q->head;
      void *data = (q->head->next)->data;
      q->head = q->head->next;
      q->qlen -= 1;
      assert(q->qlen >= 0);
      unlock_queue(q);

      free_node(n);
      return data;
} 

/*
      NOTA: in questa funzione si può accedere a q->qlen NON in mutua esclusione.
      Il rischio è quello di leggere un valore non aggiornato, ma nel caso di più
      produttori e consumatori la lunghezza della coda per un thread è comunque un
      valore "non affidabile" se non all'interno di una transazione in cui le varie
      operazioni sono tutte eseguite in mutua esclusione.
*/
unsigned long lengthQ(Queue_t *q) 
{
      lock_queue(q);
      unsigned long len = q->qlen;
      unlock_queue(q);
      return len;
}