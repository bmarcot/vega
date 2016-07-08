#ifndef MUTEX_H
#define MUTEX_H

#include "linux/types.h"

int mutex_lock(atomic_t /* __user  */ *lock);
int mutex_unlock(atomic_t /* __user */ *lock);

#endif /* !MUTEX_H */
