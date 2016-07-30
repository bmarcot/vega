#ifndef KERNEL_MUTEX_H
#define KERNEL_MUTEX_H

#include <sys/types.h>

int __pthread_mutex_lock(pthread_mutex_t *mutex);
int __pthread_mutex_unlock(pthread_mutex_t *mutex);

#endif /* !KERNEL_MUTEX_H */
