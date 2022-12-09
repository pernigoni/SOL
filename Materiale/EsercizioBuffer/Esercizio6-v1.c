#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include <time.h>
#include <sys/time.h>

#define DEBUG
#if defined(DEBUG)
#define DBG(X) X
#else
#define DBG(X)
#endif
  
static const int ITER=20; // numero di iterazioni di ogni scrittore

// variabili condivise del problema
static int  N = 100; // buffer size
static int  Ncaratteri=0;

// variabili di lock e di condizione usate per la sincronizzazione
static pthread_mutex_t mutex              = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t  SospesoInLettura   = PTHREAD_COND_INITIALIZER;
static pthread_cond_t  SospesoInScrittura = PTHREAD_COND_INITIALIZER;

#define LOCK(l)      if (pthread_mutex_lock(l)!=0)        { fprintf(stderr, "ERRORE FATALE lock\n"); exit(EXIT_FAILURE);}
#define UNLOCK(l)    if (pthread_mutex_unlock(l)!=0)      { fprintf(stderr, "ERRORE FATALE unlock\n"); exit(EXIT_FAILURE);}
#define WAIT(c,l)    if (pthread_cond_wait(c,l)!=0)       { fprintf(stderr, "ERRORE FATALE wait\n"); exit(EXIT_FAILURE);}
#define TWAIT(c,l,t) {							\
  int r=0;								\
  if ((r=pthread_cond_timedwait(c,l,t))!=0 && r!=ETIMEDOUT) {		\
    fprintf(stderr, "ERRORE FATALE timed wait\n");			\
    exit(EXIT_FAILURE);							\
  }									\
}
#define SIGNAL(c)    if (pthread_cond_signal(c)!=0)       { fprintf(stderr, "ERRORE FATALE signal\n"); exit(EXIT_FAILURE);}
#define BCAST(c)     if (pthread_cond_broadcast(c)!=0)    { fprintf(stderr, "ERRORE FATALE broadcast\n"); exit(EXIT_FAILURE);}
// ------------------------------------------------------------------


int Preleva(long id) {
  LOCK(&mutex);
  while(Ncaratteri == 0) {
    WAIT(&SospesoInLettura, &mutex);
  }
  if (Ncaratteri==-1) {
    UNLOCK(&mutex);
    return -1;
  }
  int prelevati = Ncaratteri;
  // copio nel buffer privato infinito
  Ncaratteri = 0;
  SIGNAL(&SospesoInScrittura);  
  DBG(printf("READER%ld PRELEVATI %d caratteri\n", id, prelevati));
  UNLOCK(&mutex);
  
  return prelevati;
}

void Deposita(long id, int dim) {
  LOCK(&mutex);
  while(Ncaratteri + dim > N) {
    WAIT(&SospesoInScrittura, &mutex);
  }
  // copio nel buffer dim caratteri
  Ncaratteri += dim;;
  SIGNAL(&SospesoInLettura);
  DBG(printf("WRITER%ld DEPOSITATI %d caratteri\n", id, dim));  
  UNLOCK(&mutex);
}


// funzione eseguita del generico lettore
void *Reader(void* arg) {
  long id = (long)arg;

  while(1) {
    int r = Preleva(id);
    if (r<0) break;

    // ---- simulo un po' di lavoro per il thread lettore
    struct timespec ts={0, r*1000000};
    nanosleep(&ts, NULL);
    // -------
  }
  DBG(printf("READER%ld TERMINATO\n", id));  
  return NULL;
}

// funzione eseguita del generico scrittore
void *Writer(void* arg) {
  long id = (long)arg;

  unsigned int seed = id * time(NULL);

  for(int i=0;i<ITER;++i) {
    int r = 1 + rand_r(&seed) % N;

    // ---- simulo un po' di lavoro per il thread scrittore
    int w = rand_r(&seed) % 2000;
    struct timespec ts={0, w*1000000};
    nanosleep(&ts, NULL);
    // -------
    
    Deposita(id, r);
  }
  DBG(printf("WRITER%ld TERMINATO\n", id));  
  return NULL;
}
// funzione di utilita'
static inline struct timespec after(long ms) {
  struct timespec tv;
  clock_gettime(CLOCK_REALTIME, &tv);
  tv.tv_nsec+=ms*1000000;
  if (tv.tv_nsec>=1e+9) {
    tv.tv_sec+=1;
    tv.tv_nsec-=1e+9;
  }
  return tv;
}
// Questa funzione va chiamata solo se tutti gli scrittori sono terminati 
void risvegliaLettori() {
  LOCK(&mutex);
  while(Ncaratteri>0) {
    struct timespec tv = after(30); // 30ms
    // attesa con timeout
    TWAIT(&SospesoInLettura,&mutex,&tv);
  }
  Ncaratteri = -1;
  BCAST(&SospesoInLettura); // broadcast
  UNLOCK(&mutex);
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

  pthread_t* readers     = malloc(R*sizeof(pthread_t));
  pthread_t* writers     = malloc(W*sizeof(pthread_t));
  if (!readers || !writers) {
    perror("malloc");
    fprintf(stderr, "ERRORE FATALE, esco\n");
    return -1;
  }
  // creo tutti i thread lettori 
  for(long i=0; i<R; ++i)
    if (pthread_create(&readers[i], NULL, Reader, (void*)i) != 0) {
      fprintf(stderr, "pthread_create Reader failed\n");
      return -1;
    }
  // creo tutti i thread scrittori
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
  // risveglio i lettori per farli terminare
  risvegliaLettori();

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
