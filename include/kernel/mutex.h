#ifndef KERNEL_MUTEX_H
#define KERNEL_MUTEX_H

#include "linux/types.h"

int mutex_lock(atomic_t /* __user  */ *lock);
int mutex_unlock(atomic_t /* __user */ *lock);

#endif /* !KERNEL_MUTEX_H */
