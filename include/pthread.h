#ifndef PTHREAD_H
#define PTHREAD_H

//FIXME: this is the lib, move to a lib folder

typedef unsigned int pthread_t;
typedef int pthread_mutex_t;

/* multithreading */

int pthread_yield(void);
pthread_t pthread_self(void);
void pthread_exit(void *);
int pthread_create(pthread_t *thread, /* const pthread_attr_t *attr, */
		void *(*start_routine)(void *), void *arg);

/* mutex */

#define PTHREAD_MUTEX_INITIALIZER (-1)

int pthread_mutex_lock(pthread_mutex_t *mutex);
int pthread_mutex_trylock(pthread_mutex_t *mutex);
int pthread_mutex_unlock(pthread_mutex_t *mutex);

#endif /* !PTHREAD_H */
