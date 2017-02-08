/*
 * kernel/time.c
 *
 * Copyright (c) 2016-2017 Benoit Marcot
 */

#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <kernel/bitops.h>
#include <kernel/errno-base.h>
#include <kernel/scheduler.h>
#include <kernel/signal.h>
#include <kernel/thread.h>
#include <kernel/time.h>

/* sleep functions */

//XXX: sleep() is part of <unistd.h>, but nanosleep() is part of <time.h>

extern struct thread_info *thread_idle;

static void msleep_callback(struct timer_info *timer)
{
	sched_enqueue(timer->priv); /* enqueue owner */
	CURRENT_THREAD_INFO(cur_thread);
	if (cur_thread != thread_idle)
		sched_enqueue(cur_thread);
	sched_elect(SCHED_OPT_NONE);
}

//FIXME: POSIX standard is sys_nanosleep()
int sys_msleep(unsigned int msec)
{
	struct timer_info *timer = timer_alloc();

	if (timer == NULL)
		return -1;
	CURRENT_THREAD_INFO(cur_thread);
	timer->priv = cur_thread;
	timer_configure(timer, msleep_callback);
	struct itimerspec value = {
		.it_value = { .tv_sec = msec / 1000,
			      .tv_nsec = (msec % 1000) * 1000000 } };
	timer_set(timer, &value.it_value, ONESHOT_TIMER);
	sched_dequeue(cur_thread);
	sched_elect(SCHED_OPT_NONE);
	timer_free(timer);

	return 0;
}

/* POSIX timers */

static LIST_HEAD(timer_head);

static struct timer_info *find_timer_by_id(timer_t timerid,
					struct list_head *timer_list)
{
	struct timer_info *pos;
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

int sys_timer_create(clockid_t clockid, struct sigevent *sevp,
		timer_t *timerid)
{
	(void)clockid;

	struct timer_info *timer = timer_alloc();
	if (timer == NULL)
		return -1;

	if (reserve_timer_id(&timer->id)) {
		timer_free(timer);
		return EAGAIN;
	}

	*timerid = timer->id;
	memcpy(&timer->sigev, sevp, sizeof(struct sigevent));
	list_add(&timer->list, &timer_head);

	return 0;
}

static void timer_callback(struct timer_info *timer)
{
	if (timer->it_link) {
		timer_set(timer, &timer->value.it_interval, INTERVAL_TIMER);
		timer->it_link = 0;
	}
	do_sigevent(&timer->sigev, timer->owner);
}

int sys_timer_settime(timer_t timerid, int flags,
		const struct itimerspec *new_value,
		struct itimerspec *old_value)
{
	(void)flags;

	struct timer_info *timer = find_timer_by_id(timerid, &timer_head);
	if (timer == NULL)
		return EINVAL;
	if (old_value != NULL)
		memcpy(old_value, &timer->value, sizeof(struct itimerspec));
	memcpy(&timer->value, new_value, sizeof(struct itimerspec));
	timer_configure(timer, timer_callback);

	CURRENT_THREAD_INFO(curr_thread);
	timer->owner = curr_thread;
	if (new_value->it_interval.tv_sec || new_value->it_interval.tv_nsec) {
		if ((new_value->it_value.tv_sec == new_value->it_interval.tv_sec)
			&& (new_value->it_value.tv_nsec == new_value->it_interval.tv_nsec)) {
			timer_set(timer, &new_value->it_interval, INTERVAL_TIMER);
			timer->it_link = 0;
		} else {
			timer_set(timer, &new_value->it_value, ONESHOT_TIMER);
			timer->it_link = 1;
		}
	} else {
		timer_set(timer, &new_value->it_value, ONESHOT_TIMER);
		timer->it_link = 0;
	}

	return 0;

}

int sys_timer_gettime(timer_t timerid, struct itimerspec *curr_value)
{
	struct timer_info *timer = find_timer_by_id(timerid, &timer_head);

	if (timer == NULL)
		return EINVAL;
	timer_get(timer, curr_value);

	return 0;
}
