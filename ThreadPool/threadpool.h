#ifndef _THREADPOOL_H
#define _THREADPOOL_H

#include <assert.h>
#include <utils.h>

/**
 * @brief Generico task che un thread del thread-pool deve eseguire. 
 * @param fun Puntatore alla funzione da eseguire.
 * @param arg Argomento della funzione. 
 */
typedef struct taskfun_t 
{
    void (*fun)(void *);
    void *arg;
} taskfun_t;

/**
 * @brief Rappresentazione dell'oggetto threadpool.
 */
typedef struct threadpool_t 
{
    pthread_mutex_t lock; // mutua esclusione nell'accesso all'oggetto
    pthread_cond_t cond; // usata per notificare un worker thread 
    pthread_t *threads; // array di worker id
    int numthreads; // numero di thread (size dell'array threads)
    taskfun_t *pending_queue; // coda interna per task pendenti
    int queue_size; // massima size della coda, può essere anche -1 ad indicare che non si vogliono gestire task pendenti
    int taskonthefly; // numero di task attualmente in esecuzione 
    int head, tail; // riferimenti della coda
    int count; // numero di task nella coda dei task pendenti
    int exiting; // se > 0 è iniziato il protocollo di uscita, se 1 il thread aspetta che non ci siano più lavori in coda
} threadpool_t;

/**
 * @brief Crea un oggetto thread-pool.
 * @param numthreads Numero di thread del pool.
 * @param pending_size Size delle richieste che possono essere pendenti. Questo parametro è 0 se si vuole utilizzare un modello per il pool con 1 thread 1 richiesta, cioè non ci sono richieste pendenti.
 * @return Nuovo thread-pool oppure NULL ed errno settato opportunamente.
 */
threadpool_t *create_threadpool(int numthreads, int pending_size);

/**
 * @brief Stoppa tutti i thread e distrugge l'oggetto pool.
 * @param pool Oggetto da liberare.
 * @param force Se 1 forza l'uscita immediatamente di tutti i thread e libera subito le risorse, se 0 aspetta che i thread finiscano tutti e soli i lavori pendenti (non accetta altri lavori).
 * @return 0 in caso di successo, < 0 in caso di fallimento ed errno viene settato opportunamente.
 */
int destroy_threadpool(threadpool_t *pool, int force);

/**
 * @brief Aggiunge un task al pool. 
 *      Se ci sono thread liberi il task viene assegnato ad uno di questi, 
 *      se non ci sono thread liberi e pending_size > 0 allora si cerca di inserire il task come task pendente,
 *      se non c'è posto nella coda interna allora la chiamata fallisce.
 * @param pool Oggetto thread-pool.
 * @param fun Funzione da eseguire per eseguire il task.
 * @param arg Argomento della funzione.
 * @return 0 se successo, 1 se non ci sono thread disponibili e/o la coda è piena, -1 in caso di fallimento, errno viene settato opportunamente.
 */
int add_to_threadpool(threadpool_t *pool, void (*fun)(void *),void *arg);

/**
 * @brief Lancia un thread che esegue la funzione fun passata come parametro, il thread viene lanciato in modalità detached e non fa parte del pool.
 * @param fun Funzione da eseguire per eseguire il task.
 * @param arg Argomento della funzione.
 * @return 0 se successo, -1 in caso di fallimento, errno viene settato opportunamente.
 */
int spawn_thread(void (*f)(void *), void *arg);

#endif