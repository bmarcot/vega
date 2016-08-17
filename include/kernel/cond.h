/* mutex.c
 *
 * Copyright (c) 2016 Benoit Marcot
 */

#ifndef KERNEL_CONDITION_H
#define KERNEL_CONDITION_H

#include <sys/types.h>

int __pthread_cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex);
int __pthread_cond_signal(pthread_cond_t *cond);

#endif /* !KERNEL_CONDITION_H */
