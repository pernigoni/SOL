/*
 * NOTA: 
 * In questa versione non viene gestita la terminazione dei thread
 * produttori e consumatori. 
 * L'implementazione della terminazione viene lasciata per esercizio.
 *
 *
 */
#define _POSIX_C_SOURCE 200112L
#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/time.h>

#define DEBUG
#if defined(DEBUG)
#define DBG(X) X
#else
#define DBG(X)
#endif

static int  N = 100; // buffer size
static int Ncaratteri=0;

// semafori usati per la sincronizzazione
static sem_t mutex;
static sem_t attesaCaratteri;
static sem_t attesaSpazio;

static inline void P(sem_t* sem) {
  if (sem_wait(sem)==-1) {
    perror("P");
    fprintf(stderr, "ERRORE FATALE, exit\n");
    exit(EXIT_FAILURE);
  }
}
static inline void V(sem_t* sem) {
  if (sem_post(sem)==-1) {
    perror("V");
    fprintf(stderr, "ERRORE FATALE, exit\n");
    exit(EXIT_FAILURE);
  }
}
// ------------------------------------------------------------------

int Preleva(long id) {
  int OK = 0;
  int prelevati = 0;
  do {
    P(&mutex);
    if (Ncaratteri > 0) {
      OK = 1;
      prelevati = Ncaratteri;
      Ncaratteri = 0;
      DBG(fprintf(stderr,"READER%ld PRELEVATI %d caratteri\n", id, prelevati));  
    } 
    V(&mutex);
    if (!OK)
      P(&attesaCaratteri);    
  } while(!OK);
  V(&attesaSpazio);
  return prelevati;
}

void Deposita(long id, int dim) {
  int OK = 0; 
  do {
    P(&mutex);
    if (Ncaratteri + dim <= N) {
      OK = 1;
      Ncaratteri += dim;
      DBG(fprintf(stderr,"WRITER%ld DEPOSITATI %d caratteri\n", id, dim));  
    }
    V(&mutex);
    if (!OK) P(&attesaSpazio);
  } while(!OK);
  V(&attesaCaratteri);
}

// funzione eseguita del generico lettore
void *Reader(void* arg) {
  long id = (long)arg;

  while(1) {
	int r=Preleva(id);

    // ---- simulo un po' di lavoro per il thread lettore
    struct timespec ts={0, r*1000000};
    nanosleep(&ts, NULL);
    // -------
  }
  DBG(fprintf(stderr,"READER%ld TERMINATO\n", id));  
  return NULL;
}

// funzione eseguita del generico scrittore
void *Writer(void* arg) {
  long id = (long)arg;

  unsigned int seed = id * time(NULL);

  while(1) {
    int r = 1 + rand_r(&seed) % N;

    // ---- simulo un po' di lavoro per il thread scrittore
    int w = rand_r(&seed) % 2000;
    struct timespec ts={0, w*1000000};
    nanosleep(&ts, NULL);
    // -------
    
    Deposita(id, r);
  }
  DBG(fprintf(stderr,"WRITER%ld TERMINATO\n", id));  
  return NULL;
}

int main(int argc, char* argv[]) {
  int R=5;
  int W=3;
  if (argc > 1) {
    if (argc != 3 && argc != 4) {
      fprintf(stderr, "usage: %s [#R #W bufsize]\n", argv[0]);
      return -1;
    }
    R = atoi(argv[1]);
    W = atoi(argv[2]);
    if (argc == 4)
      N = atoi(argv[3]);
  }  

  // metto un limite ragionevole per i lettori e scrittori
  if (R>100) R = 100;
  if (W>100) W = 100;
  assert(N>0);
  
  // sem initializations
  if (sem_init(&attesaCaratteri, 0, 0)<0) {
    fprintf(stderr, "sem_init attesaCaratteri failed\n");
    return -1;
  }
  if (sem_init(&attesaSpazio, 0, 0)<0) {
    fprintf(stderr, "sem-init attesaSpazio failed\n");
    return -1;
  }	   
  if (sem_init(&mutex, 0, 1)<0) {
    fprintf(stderr, "sem_init mutex failed\n");
    return -1;
  }
  
  pthread_t* readers     = malloc(R*sizeof(pthread_t));
  pthread_t* writers     = malloc(W*sizeof(pthread_t));
  if (!readers || !writers) {
    fprintf(stderr, "not enough memory\n");
    return -1;
  }

  for(long i=0; i<R; ++i)
    if (pthread_create(&readers[i], NULL, Reader, (void*)i) != 0) {
      fprintf(stderr, "pthread_create Reader failed\n");
      return -1;
    }
  for(long i=0; i<W; ++i)
    if (pthread_create(&writers[i], NULL, Writer, (void*)i) != 0) {
      fprintf(stderr, "pthread_create Writer failed\n");
      return -1;
    }
  // aspetto che gli scrittori terminino
  for(long i=0;i<W; ++i)
    if (pthread_join(writers[i], NULL) == -1) {
      fprintf(stderr, "pthread_join failed\n");
    }
  // aspetto che i lettori terminino
  for(long i=0;i<R; ++i)
    if (pthread_join(readers[i], NULL) == -1) {
      fprintf(stderr, "pthread_join failed\n");
    }
  // libero memoria ed esco
  free(writers);
  free(readers);
  return 0;   
  
}
