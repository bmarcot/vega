/*
 * include/kernel/hrtimer.h
 *
 * Copyright (c) 2017-2018 Benoit Marcot
 */

#ifndef _KERNEL_HRTIMER_H
#define _KERNEL_HRTIMER_H

#include <kernel/types.h>

#include <asm/ktime.h>

/*
 * Return values for the callback function
 */
enum hrtimer_restart {
	HRTIMER_NORESTART,	/* Timer is not restarted */
	HRTIMER_RESTART,	/* Timer must be restarted */
};

/*
 * Values to track state of the timer
 */
enum hrtimer_state {
	HRTIMER_STATE_INACTIVE,
	HRTIMER_STATE_ENQUEUED,
};

struct clock_event_device;

struct hrtimer {
	struct list_head		list;
	ktime_t				expires;
	enum hrtimer_restart		(*function)(struct hrtimer *);
	struct clock_event_device	*dev;
	enum hrtimer_state		state;
};

int hrtimer_start(struct hrtimer *timer, ktime_t expires);
int hrtimer_cancel(struct hrtimer *timer);
void hrtimer_init(struct hrtimer *timer);

//XXX: Will die.. Function unused from kernel..
struct hrtimer *hrtimer_alloc(void);

#endif /* !_KERNEL_HRTIMER_H */
