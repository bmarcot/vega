/*
 * kernel/time.c
 *
 * Copyright (C) 2016 Benoit Marcot
 */

#include <signal.h>
#include <stdlib.h>
#include <string.h>

#include <kernel/bitops.h>
#include <kernel/errno-base.h>
#include <kernel/scheduler.h>
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
	/* printk("timer: create a timer t=%dms at %p\n", msec, &timer); */
	CURRENT_THREAD_INFO(cur_thread);
	timer->priv = cur_thread;
	timer_configure(timer, msleep_callback);
	timer_set(timer, msec * 1000);
	sched_dequeue(cur_thread);
	sched_elect(SCHED_OPT_NONE);
	timer_free(timer);

	return 0;
}

/* POSIX timers */

static LIST_HEAD(kernel_timers);

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

	/* printk(">> sys_ timer_create()\n"); */

	struct timer_info *timer = timer_alloc();
	if (timer == NULL)
		return -1;
	struct sigevent *sigev = malloc(sizeof(struct sigevent));
	if (sigev == NULL) {
		timer_free(timer);
		return ENOMEM;
	}
	if (reserve_timer_id(&timer->id)) {
		timer_free(timer);
		free(sigev);
		return EAGAIN;
	}
	*timerid = timer->id;
	timer->priv = sigev;
	memcpy(sigev, sevp, sizeof(struct sigevent));
	list_add(&timer->list, &kernel_timers);

	return 0;
}

void do_sigevent(const struct sigevent *sigevent);

static void timer_callback(struct timer_info *timer)
{
	/* printk(">> timer_callback()\n"); */
	/* FIXME: Sigevent handler is staged in current thread
	   at the moment. It should be staged in timer's owner
	   thread instead. */
	do_sigevent(timer->priv);
}

/* int timer_settime(timer_t timerid, int flags, */
/* 		const struct itimerspec *new_value, */
/* 		struct itimerspec * old_value) */
int sys_timer_settime(timer_t timerid, int flags, int new_value)
{
	(void)flags;

	/* printk(">> sys_ timer_settime()\n"); */

	struct timer_info *timer = find_timer_by_id(timerid, &kernel_timers);
	if (timer == NULL) {
		printk("timer_settime: No timer found with id=%d\n", timerid);
		return EINVAL;
	}
	timer_configure(timer, timer_callback);
	timer_set(timer, new_value);

	return 0;

}
