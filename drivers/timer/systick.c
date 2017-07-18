/*
 * drivers/timer/systick.c
 *
 * Copyright (c) 2016-2017 Benoit Marcot
 *
 * Low-resolution (1kHz) SysTick-based timers to support the common
 * driver's timer interface.
 */

#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <kernel/errno-base.h>
#include <kernel/list.h>
#include <kernel/time.h>

struct systick_timer {
	unsigned long     start_clocktime;
	unsigned long     expire_clocktime;
	struct list_head  list;
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

int systick_timer_set(struct timer_info *timer, const struct timespec *value)
{
	struct systick_timer *systick_timer =
		(struct systick_timer *)timer->dev;

	if (!value->tv_sec && !value->tv_nsec)
		list_del(&systick_timer->list);
	if (value->tv_sec || value->tv_nsec) {
		systick_timer->start_clocktime = clocktime_in_msec;
		systick_timer->expire_clocktime = clocktime_in_msec
			+ value->tv_sec * 1000 + value->tv_nsec / 1000000;
		list_add(&systick_timer->list, &systick_timers);
	}

	return 0;
}

int systick_timer_get(struct timer_info *timer, struct itimerspec *curr_value)
{
	struct systick_timer *systick_timer =
		(struct systick_timer *)timer->dev;
	unsigned long msecs = clocktime_in_msec - systick_timer->start_clocktime;

	curr_value->it_value.tv_sec = msecs / 1000;
	curr_value->it_value.tv_nsec = (msecs % 1000) * 1000000;

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
			list_del(&pos->list);
			struct timer_info *timer = pos->timer;
			if (timer->type == INTERVAL_TIMER)
				systick_timer_set(timer,
						&timer->value.it_interval);
			timer_expire_callback(timer);
		}
	}
}

const struct timer_operations systick_tops = {
	.timer_alloc = systick_timer_alloc,
	.timer_set = systick_timer_set,
	.timer_get = systick_timer_get,
	.timer_free = systick_timer_free,
};
