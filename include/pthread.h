#ifndef PTHREAD_H
#define PTHREAD_H

typedef unsigned int pthread_t;

int pthread_yield(void);
pthread_t pthread_self(void);

#endif /* !PTHREAD_H */
