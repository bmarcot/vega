#ifndef PTHREAD_H
#define PTHREAD_H

#include <sys/pthread_types.h>
#include <sys/types.h>

/* pthread attributes */

int pthread_attr_init(pthread_attr_t *attr);
int pthread_attr_setstacksize(pthread_attr_t *attr, size_t stacksize);
int pthread_attr_getstacksize(const pthread_attr_t *attr, size_t *stacksize);

/* multithreading */

int pthread_yield(void);
pthread_t pthread_self(void);
void pthread_exit(void *);
int pthread_create(pthread_t *thread, const pthread_attr_t *attr,
		void *(*start_routine)(void *), void *arg);
int pthread_join(pthread_t thread, void **retval);
int pthread_detach(pthread_t thread);

/* mutex */

#define PTHREAD_MUTEX_UNLOCKED        1
#define PTHREAD_MUTEX_LOCKED          0
#define PTHREAD_MUTEX_WAITERS_QUEUED -1 /* locked, possible waiters queued */

#define PTHREAD_MUTEX_INITIALIZER PTHREAD_MUTEX_UNLOCKED

int pthread_mutex_init(pthread_mutex_t *mutex, const pthread_mutexattr_t *attr);
int pthread_mutex_lock(pthread_mutex_t *mutex);
int pthread_mutex_trylock(pthread_mutex_t *mutex);
int pthread_mutex_unlock(pthread_mutex_t *mutex);
int __pthread_mutex_lock(pthread_mutex_t *mutex);
int __pthread_mutex_trylock(pthread_mutex_t *mutex);
int __pthread_mutex_unlock(pthread_mutex_t *mutex);

/* cond variable */

int pthread_cond_init(pthread_cond_t *cond, const pthread_condattr_t *attr);
int pthread_cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex);
int pthread_cond_signal(pthread_cond_t *cond);

#endif /* !PTHREAD_H */
