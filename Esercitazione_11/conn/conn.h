#ifndef _CONN_H
#define _CONN_H

#include <sys/types.h> 
#include <sys/socket.h>
#include <sys/uio.h>
#include <sys/un.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#ifndef SOCKNAME
#define SOCKNAME "./cs_sock"
#endif
#ifndef MAXBACKLOG
#define MAXBACKLOG 32
#endif

/** Evita letture parziali
 *
 *   \retval -1   errore (errno settato)
 *   \retval  0   se durante la lettura da fd leggo EOF
 *   \retval size se termina con successo
 */
int readn(long fd, void *buf, size_t size);

/** Evita scritture parziali
 *
 *   \retval -1   errore (errno settato)
 *   \retval  0   se durante la scrittura la write ritorna 0
 *   \retval  1   se la scrittura termina con successo
 */
int writen(long fd, void *buf, size_t size);

#endif
