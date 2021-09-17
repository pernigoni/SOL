#ifndef _QUEUE_H
#define _QUEUE_H

#include <pthread.h>

// Elemento della coda
typedef struct Node 
{
      void *data;
      struct Node *next;
} Node_t;

// Struttura dati coda
typedef struct Queue 
{
      Node_t *head;
      Node_t *tail;    
      unsigned long qlen;    
      pthread_mutex_t qlock;
      pthread_cond_t qcond;
} Queue_t;

/*    
      Alloca e inizializza una coda.
      Deve essere chiamata da un solo thread (tipicamente il thread main).
      Ritorna il puntatore alla coda allocata o NULL in caso di errore (setta errno).
*/
Queue_t* initQueue();

/*
      Cancella la coda puntata da q.
      Deve essere chiamata da un solo thread (tipicamente il thread main).
*/
void deleteQueue(Queue_t *q);

/*
      Inserisce un dato nella coda.
      Ritorna 0 in caso di successo, -1 altrimenti (setta errno).
*/
int push(Queue_t *q, void *data);

/*
      Estrae un dato dalla coda.
      Ritorna il puntatore al dato estratto.
*/
void* pop(Queue_t *q);

/*
      Ritorna la lunghezza della coda passata come parametro.
*/
unsigned long lengthQ(Queue_t *q);

#endif