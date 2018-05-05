/*
 * kernel/time/hrtimer.c
 *
 * Copyright (c) 2017-2018 Benoit Marcot
 */

#include <kernel/errno-base.h>
#include <kernel/ktime.h>
#include <kernel/list.h>
#include <kernel/mm.h>
#include <kernel/sched.h>
#include <kernel/signal.h>
#include <kernel/string.h>
#include <kernel/syscalls.h>
#include <kernel/time.h>
#include <kernel/time/clockevents.h>
#include <kernel/time/hrtimer.h>

#include <asm/current.h>
#include <asm/ktime.h>

static LIST_HEAD(hrtimers);

int hrtimer_set_expires(struct hrtimer *timer, ktime_t expires)
{
	struct hrtimer *t;

	/* cancel timer */
	if (!expires) {
		list_del(&timer->list);
		return clockevents_program_event(timer->dev, 0);
	}

	timer->expires = expires + clockevents_read_elapsed(timer->dev);

	/* timer list is empty or timer is the new head */
	t = list_first_entry_or_null(&hrtimers, struct hrtimer, list);
	if (!t || (t->expires > timer->expires)) {
		list_add(&timer->list, &hrtimers);
		return clockevents_program_event(timer->dev, expires);
	}

	list_for_each_entry(t, &hrtimers, list) {
		if (t->expires > timer->expires) {
			list_add_tail(&timer->list, &t->list);
			return 0;
		}
	}
	list_add_tail(&timer->list, &hrtimers);

	return 0;
}

static void hrtimer_interrupt(struct clock_event_device *dev)
{
	struct hrtimer *timer;
	struct hrtimer *next;

	timer = list_first_entry_or_null(&hrtimers, struct hrtimer, list);
	if (!timer)
		return; //XXX: Something went wrong
	list_del(&timer->list);

	/* program next timer event */
	next = list_first_entry_or_null(&hrtimers, struct hrtimer, list);
	if (next) {
		ktime_t expires = next->expires - timer->expires;
		clockevents_program_event(next->dev, expires);
	}

	if (timer->callback)
		timer->callback(timer->context);
}

int hrtimer_init(struct hrtimer *timer)
{
	timer->state = HRTIMER_STATE_INACTIVE;
	timer->dev = clockevents_get_device(HRTIMER_DEVICE);
	if (!timer->dev)
		return -1;
	clockevent_set_event_handler(timer->dev, hrtimer_interrupt);
	clockevents_switch_state(timer->dev, CLOCK_EVT_STATE_ONESHOT);

	return 0;
}

struct hrtimer *hrtimer_alloc(void)
{
	struct hrtimer *timer = kzalloc(sizeof(*timer));

	if (timer && hrtimer_init(timer))
		kfree(timer);

	return timer;
}

static void nanosleep_callback(void *task)
{
	sched_enqueue(task);
	schedule();
}

SYSCALL_DEFINE(nanosleep,
	const struct timespec	*req,
	struct timespec		*rem)
{
	struct hrtimer timer;

	if (hrtimer_init(&timer))
		return -1;
	timer.callback = nanosleep_callback;
	timer.context = current;
	if (hrtimer_set_expires(&timer, timespec_to_ktime(*req)))
		return -1;

	sched_dequeue(current);
	current->state = TASK_INTERRUPTIBLE;

	schedule();

	if (signal_pending(current)) {
		ktime_t elapsed = clockevents_read_elapsed(timer.dev);
		*rem = ktime_to_timespec(timer.expires - elapsed);
		return -EINTR;
	}

	memset(rem, 0, sizeof(*rem));

	return 0;
}
