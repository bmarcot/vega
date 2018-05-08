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

#endif /* !_KERNEL_TIME_H */
