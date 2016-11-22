/*
 * drivers/timer/systick.c
 *
 * Copyright (C) 2016 Benoit Marcot
 *
 * Low-resolution (1kHz) SysTick-based timers to support the common
 * timer interface.
 */

#include <errno.h>
#include <stdlib.h>

#include <kernel/errno-base.h>
#include <kernel/time.h>

#include "linux/list.h"

struct systick_timer {
	unsigned long expire_clocktime;
	struct list_head list;
	struct timer_info *timer; /* backlink */
};

static unsigned long clocktime_in_msec;

static LIST_HEAD(systick_timers);

int systick_timer_alloc(struct timer_info *timer)
{
	struct systick_timer *systick_timer;

	systick_timer = malloc(sizeof(struct systick_timer));
	if (systick_timer == NULL) {
		errno = ENOMEM;
		return -1;
	}
	timer->dev = systick_timer;
	systick_timer->timer = timer;

	return 0;
}

int systick_timer_configure(struct timer_info *timer,
			void (*callback)(struct timer_info *self))
{
	(void)timer, (void)callback;

	return 0;
}

int systick_timer_set(struct timer_info *timer, unsigned int usec)
{
	struct systick_timer *systick_timer =
		(struct systick_timer *)timer->dev;

	systick_timer->expire_clocktime = clocktime_in_msec + usec / 1000;
	list_add(&systick_timer->list, &systick_timers);

	return 0;
}

int systick_timer_cancel(struct timer_info *timer)
{
	(void)timer;

	return 0;
}

int systick_timer_free(struct timer_info *timer)
{
	free(timer->dev);

	return 0;
}

#define SYSTICK_FREQ_IN_HZ 1000
#define SYSTICK_PERIOD_IN_MSECS (SYSTICK_FREQ_IN_HZ / 1000)

void systick(void)
{
	clocktime_in_msec += SYSTICK_PERIOD_IN_MSECS;

	//XXX: is list_for_each_entry_safe() reentrant?
	struct systick_timer *pos, *pos1;
	list_for_each_entry_safe(pos, pos1, &systick_timers, list) {
		if (pos->expire_clocktime < clocktime_in_msec) {
			//printk("timer expired!\n");
			list_del(&pos->list);
			pos->timer->callback(pos->timer);
		}
	}
}

struct timer_operations systick_tops = {
	.timer_alloc = systick_timer_alloc,
	.timer_configure = systick_timer_configure,
	.timer_set = systick_timer_set,
	.timer_cancel = systick_timer_cancel,
	.timer_free = systick_timer_free,
};
