#ifndef TIMER_H
#define TIMER_H

#include "thread.h"
#include "linux/types.h"
#include "linux/list.h"

struct timer {
	struct thread_info *tip;
	u32 expire_clocktime;
	struct list_head list;
};

/* system interfaces */
int timer_create(unsigned int msecs);
int timer_create_1(unsigned int msecs);

#endif /* !TIMER_H */
