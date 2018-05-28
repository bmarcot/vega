/*
 * include/kernel/time.h
 *
 * Copyright (c) 2016-2018 Benoit Marcot
 */

#ifndef _KERNEL_TIME_H
#define _KERNEL_TIME_H

#include <kernel/hrtimer.h>
#include <kernel/signal.h>
#include <kernel/types.h>

#include <uapi/kernel/time.h>

#define MSEC_PER_SEC	1000l
#define USEC_PER_MSEC	1000l
#define NSEC_PER_USEC	1000l
#define NSEC_PER_MSEC	1000000l
#define USEC_PER_SEC	1000000l
#define NSEC_PER_SEC	1000000000l

struct task_struct;

struct posix_timer {
	//XXX: Will refactor those fields
	struct sigqueue		sigqueue; // sigqueue or sigevent
	struct list_head	list;     // Duplicates list in hrtimer

	timer_t			id;
	u32			flags;
	struct itimerspec	value;
	struct task_struct	*task;
	struct hrtimer		timer;
};

int time_init(void);

#endif /* !_KERNEL_TIME_H */
