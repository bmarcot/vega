/*
 * kernel/time/posix-stubs.c
 *
 * Copyright (c) 2016-2017 Benoit Marcot
 */

#include <string.h>

#include <kernel/bitops.h>
#include <kernel/errno-base.h>
#include <kernel/ktime.h>
#include <kernel/list.h>
#include <kernel/mm.h>
#include <kernel/sched.h>
#include <kernel/signal.h>
#include <kernel/syscalls.h>
#include <kernel/time.h>
#include <kernel/time/clocksource.h>

#include <uapi/kernel/time.h>

#include <asm/current.h>

static LIST_HEAD(posix_timers);

static struct posix_timer *find_timer_by_id(timer_t timerid,
					struct list_head *timer_list)
{
	struct posix_timer *pos;
	list_for_each_entry(pos, timer_list, list) {
		if (pos->id == timerid)
			return pos;
	}

	return NULL;
}

static int reserve_timer_id(timer_t *timerid)
{
	static unsigned long bitmap = 0;

	unsigned long bit = find_first_zero_bit(&bitmap, BITS_PER_LONG);
	if (bit == BITS_PER_LONG)
		return -1;
	bitmap_set_bit(&bitmap, bit);
	*timerid = bit;

	return 0;
}

static void timer_callback(void *context)
{
	struct posix_timer *timer = context;

	if (timer->type == ONESHOT_TIMER)
		timer->disarmed = 1;
	signal_event(timer->owner, &timer->sigev);
}

static void timer_callback_and_link(void *context)
{
	struct posix_timer *timer = context;

	hrtimer_set_expires(&timer->hrtimer,
			timespec_to_ktime(timer->value.it_interval));
	signal_event(timer->owner, &timer->sigev);
}

SYSCALL_DEFINE(timer_create,
	clockid_t		clockid,
	struct sigevent		*sevp,
	timer_t			*timerid)
{
	(void)clockid;

	struct posix_timer *pt = kzalloc(sizeof(*pt));
	if (!pt)
		return -1;

	hrtimer_init(&pt->hrtimer);
	pt->hrtimer.context = pt;

	if (reserve_timer_id(&pt->id)) {
		kfree(pt);
		return EAGAIN;
	}

	*timerid = pt->id;
	pt->disarmed = 1;
	memcpy(&pt->sigev, sevp, sizeof(struct sigevent));
	list_add(&pt->list, &posix_timers);

	return 0;
}

SYSCALL_DEFINE(timer_settime,
	timer_t			timerid,
	int			flags,
	const struct itimerspec	*new_value,
	struct itimerspec	*old_value)
{
	(void)flags;

	struct posix_timer *timer = find_timer_by_id(timerid, &posix_timers);
	ktime_t expires;

	if (!timer)
		return EINVAL;
	if (old_value)
		memcpy(old_value, &timer->value, sizeof(struct itimerspec));
	memcpy(&timer->value, new_value, sizeof(struct itimerspec));

	/* disarm timer */
	if (!new_value->it_value.tv_sec && !new_value->it_value.tv_nsec)
	{
		if (!timer->disarmed) {
			hrtimer_set_expires(&timer->hrtimer, 0);
			timer->disarmed = 1;
		}
		return 0;
	}

	timer->owner = current;
	timer->disarmed = 0;

	if (new_value->it_interval.tv_sec || new_value->it_interval.tv_nsec) {
		timer->type = INTERVAL_TIMER;
		timer->hrtimer.callback = timer_callback_and_link;
		if (new_value->it_value.tv_sec || new_value->it_value.tv_nsec)
			expires = timespec_to_ktime(new_value->it_value);
		else
			expires = timespec_to_ktime(new_value->it_interval);
		hrtimer_set_expires(&timer->hrtimer, expires);
	} else {
		timer->type = ONESHOT_TIMER;
		timer->hrtimer.callback = timer_callback;
		expires = timespec_to_ktime(new_value->it_value);
		hrtimer_set_expires(&timer->hrtimer, expires);
	}

	return 0;
}

SYSCALL_DEFINE(timer_gettime,
	timer_t			timerid,
	struct itimerspec	*curr_value)
{
	struct posix_timer *pt = find_timer_by_id(timerid, &posix_timers);
	ktime_t expires = pt->hrtimer.expires;
	ktime_t now = clock_monotonic_read();

	if (!pt)
		return EINVAL;
	if (pt->disarmed || (now >= expires)) {
		memset(&curr_value->it_value, 0, sizeof(struct timespec));
	} else {
		struct timespec ts = ktime_to_timespec(expires - now);
		memcpy(&curr_value->it_value, &ts, sizeof(ts));
	}

	return 0;
}

SYSCALL_DEFINE(clock_gettime,
	clockid_t		clk_id,
	struct timespec		*tp)
{
	struct timespec ts;

	if (clk_id == CLOCK_MONOTONIC) {
		ts = ktime_to_timespec(clock_monotonic_read());
		memcpy(tp, &ts, sizeof(*tp));
	}

	return 0;
}
