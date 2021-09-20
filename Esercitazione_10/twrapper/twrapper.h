#ifndef _TWRAPPER_H
#define _TWRAPPER_H

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

void safe_pthread_create(pthread_t *tid, const pthread_attr_t *attr, void *(*start_routine) (void *), void *arg);

void safe_pthread_mutex_lock(pthread_mutex_t *mtx);

void safe_pthread_mutex_unlock(pthread_mutex_t *mtx);

void safe_pthread_cond_wait(pthread_cond_t *cond, pthread_mutex_t *mtx);

void safe_pthread_cond_signal(pthread_cond_t* cond);

#endif