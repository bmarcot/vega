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

enum hrtimer_state {
	HRTIMER_STATE_INACTIVE,
	HRTIMER_STATE_ENQUEUED,
};

struct hrtimer {
	ktime_t			expires;
	unsigned int		flags;
	struct list_head	list;
	enum hrtimer_state	state;

	struct clock_event_device *dev;

	void			(*callback) (void *);
	void			*context;
};

int hrtimer_set_expires(struct hrtimer *timer, ktime_t expires);
int hrtimer_init(struct hrtimer *timer);
struct hrtimer *hrtimer_alloc(void);

#endif /* !_KERNEL_TIME_HRTIMER_H */
