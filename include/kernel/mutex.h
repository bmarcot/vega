/* include/kernel/mutex.h
 *
 * Copyright (c) 2016 Benoit Marcot
 */

#ifndef KERNEL_MUTEX_H
#define KERNEL_MUTEX_H

#include <sys/types.h>

int sys_pthread_mutex_lock(pthread_mutex_t *mutex);
int sys_pthread_mutex_unlock(pthread_mutex_t *mutex);

#endif /* !KERNEL_MUTEX_H */
