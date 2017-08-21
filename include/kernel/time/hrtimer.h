/*
 * include/kernel/time/hrtimer.h
 *
 * Copyright (c) 2017 Benoit Marcot
 */

#ifndef _KERNEL_TIME_HRTIMER_H
#define _KERNEL_TIME_HRTIMER_H

#include <kernel/types.h>

#include <asm/ktime.h>

struct  clock_event_device;

struct hrtimer {
	ktime_t			expires;
	unsigned int		flags;
	struct list_head	list;

	struct clock_event_device *dev;

	void			(*callback) (void *);
	void			*context;
};

#endif /* !_KERNEL_TIME_HRTIMER_H */
