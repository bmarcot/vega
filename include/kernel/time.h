/*
 * include/kernel/time.h
 *
 * Copyright (c) 2016 Benoit Marcot
 */

#ifndef KERNEL_TIME_H
#define KERNEL_TIME_H

#include <sys/types.h>
#include "linux/list.h"

struct thread_info;

enum timer_type {
	TT_SLEEP,
	TT_TIMER
};

struct timer_common {
	timer_t            timerid;  //FIXME: harvest storage, timer_id + timer_type
	u32                expire_clocktime;
	enum timer_type    timer_type;
	union {
		struct thread_info *owner;  /* for sleep */
		struct sigevent *sigev;     /* for timer */
	};
};

#endif /* !KERNEL_TIME_H */
