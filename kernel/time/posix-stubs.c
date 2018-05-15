/*
 * kernel/time/posix-stubs.c
 *
 * Copyright (c) 2016-2018 Benoit Marcot
 */

#include <kernel/bitops.h>
#include <kernel/clockevents.h>
#include <kernel/clocksource.h>
#include <kernel/errno-base.h>
#include <kernel/hrtimer.h>
#include <kernel/ktime.h>
#include <kernel/list.h>
#include <kernel/mm.h>
#include <kernel/mm/slab.h>
#include <kernel/sched.h>
#include <kernel/signal.h>
#include <kernel/string.h>
#include <kernel/syscalls.h>
#include <kernel/time.h>

#include <uapi/kernel/time.h>

#include <asm/current.h>

static LIST_HEAD(posix_timers);

static struct kmem_cache *posix_timer_cache;

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

static enum hrtimer_restart timer_callback(struct hrtimer *timer)
{
	struct posix_timer *pt =
		container_of(timer, struct posix_timer, timer);

	//XXX: SIGEV_THREAD unsupported
	struct sigqueue *sig = &pt->sigqueue;
	if (sig->info.si_signo)
		send_signal_info(sig->info.si_signo, sig, pt->task); //XXX: task can be different from signal receiver

	if (pt->value.it_interval.tv_sec || pt->value.it_interval.tv_nsec) {
		pt->timer.expires = timespec_to_ktime(pt->value.it_interval);
		return HRTIMER_RESTART;
	}

	return HRTIMER_NORESTART;
}

SYSCALL_DEFINE(timer_create,
	clockid_t		clockid,
	struct sigevent		*sevp,
	timer_t			*timerid)
{
	struct posix_timer *pt;
	siginfo_t *info;

	pt = kmem_cache_alloc(posix_timer_cache, CACHE_OPT_NONE);
	if (!pt)
		return -ENOMEM;

	/* Initialise the actual timer */
	hrtimer_init(&pt->timer);
	pt->timer.function = timer_callback;

	if (reserve_timer_id(&pt->id)) {
		kfree(pt);
		return EAGAIN;
	}

	*timerid = pt->id;
	pt->task = current;
	pt->sigqueue.flags = SIGQUEUE_PREALLOC;
	info = &pt->sigqueue.info;
	info->si_signo = sevp->sigev_signo;
	info->_timer.si_value.sival_int = sevp->sigev_value.sival_int;
	list_add(&pt->list, &posix_timers);

	return 0;
}

SYSCALL_DEFINE(timer_settime,
	timer_t			timerid,
	int			flags,
	const struct itimerspec	*new_value,
	struct itimerspec	*old_value)
{
	struct posix_timer *pt = find_timer_by_id(timerid, &posix_timers);

	/* timerid is invalid */
	if (!pt)
		return EINVAL;

	if (old_value)
		memcpy(old_value, &pt->value, sizeof(struct itimerspec));
	memcpy(&pt->value, new_value, sizeof(struct itimerspec));

	if (new_value->it_value.tv_sec || new_value->it_value.tv_nsec)
		hrtimer_start(&pt->timer, timespec_to_ktime(new_value->it_value));
	else
		hrtimer_cancel(&pt->timer);

	return 0;
}

SYSCALL_DEFINE(timer_gettime,
	timer_t			timerid,
	struct itimerspec	*curr_value)
{
	struct posix_timer *pt = find_timer_by_id(timerid, &posix_timers);
	ktime_t expires = pt->timer.expires;
	ktime_t elapsed = clockevents_read_elapsed(pt->timer.dev);

	if (!pt)
		return EINVAL;

	if ((pt->timer.state == HRTIMER_STATE_INACTIVE) || (elapsed >= expires)) {
		memset(&curr_value->it_value, 0, sizeof(struct timespec));
	} else {
		struct timespec ts = ktime_to_timespec(expires - elapsed);
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

int time_init(void)
{
	posix_timer_cache = KMEM_CACHE(posix_timer);
	BUG_ON(!posix_timer_cache);

	return 0;
}
