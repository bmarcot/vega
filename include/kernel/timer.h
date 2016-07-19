#ifndef KERNEL_TIMER_H
#define KERNEL_TIMER_H

#include <sys/types.h>

#include <kernel/thread.h>

#include "linux/types.h"
#include "linux/list.h"

struct timer {
	struct thread_info *owner;
	u32 expire_clocktime;
	struct list_head list;  /* ordering list */
};

/* system interfaces */
int timer_create(unsigned int msecs);

#endif /* !KERNEL_TIMER_H */
