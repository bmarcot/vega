/*
 * kernel/time.c
 *
 * Copyright (C) 2016 Benoit Marcot
 */

#include <kernel/scheduler.h>
#include <kernel/thread.h>
#include <kernel/time.h>

//XXX: sleep() is part of <unistd.h>, but nanosleep() is part of <time.h>

extern struct thread_info *thread_idle;

static void expire_callback(struct timer_info *timer)
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
	printk("timer: create a timer t=%dms at %p\n", msec, &timer);
	CURRENT_THREAD_INFO(cur_thread);
	timer->priv = cur_thread;
	timer_configure(timer, expire_callback);
	timer_set(timer, msec * 1000);
	sched_dequeue(cur_thread);
	sched_elect(SCHED_OPT_NONE);
	timer_free(timer);

	return 0;
}
