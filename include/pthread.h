#ifndef PTHREAD_H
#define PTHREAD_H

//FIXME: this is the lib, move to a lib folder

typedef unsigned int pthread_t;

int pthread_yield(void);
pthread_t pthread_self(void);
void pthread_exit(void *);
int pthread_create(pthread_t *thread, /* const pthread_attr_t *attr, */
		void *(*start_routine)(void *), void *arg);

#endif /* !PTHREAD_H */
