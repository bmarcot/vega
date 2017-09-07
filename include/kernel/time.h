/*
 * include/kernel/time.h
 *
 * Copyright (c) 2016-2017 Benoit Marcot
 */

#ifndef _KERNEL_TIME_H
#define _KERNEL_TIME_H

#include <kernel/sched.h>
#include <kernel/time/hrtimer.h>
#include <kernel/types.h>

#include <uapi/kernel/signal.h>
#include <uapi/kernel/time.h>

enum timer_type { ONESHOT_TIMER, INTERVAL_TIMER };

struct posix_timer {
	timer_t			id;
	u32			flags;
	int			disarmed;
	enum timer_type		type;

	void		(*callback)(struct posix_timer *self); //XXX: This will die

	struct task_struct	*owner;
	struct itimerspec	value;
	struct sigevent		sigev;
	struct list_head	list;
	struct hrtimer		hrtimer;
};

#endif /* !_KERNEL_TIME_H */
