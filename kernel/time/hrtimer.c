/*
 * kernel/time/hrtimer.c
 *
 * Copyright (c) 2017 Benoit Marcot
 */

#include <kernel/ktime.h>
#include <kernel/list.h>
#include <kernel/mm.h>
#include <kernel/sched.h>
#include <kernel/syscalls.h>
#include <kernel/time.h>
#include <kernel/time/clockevents.h>
#include <kernel/time/clocksource.h>
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

	timer->expires = expires + clock_monotonic_read();

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
	(void)dev;

	struct hrtimer *timer;
	struct hrtimer *next;

	timer = list_first_entry_or_null(&hrtimers, struct hrtimer, list);
	if (!timer)
		return; //XXX: Something went wrong
	list_del(&timer->list);

	/* program next timer event */
	next = list_first_entry_or_null(&hrtimers, struct hrtimer, list);
	if (next) {
		ktime_t expires = next->expires - clock_monotonic_read();
		clockevents_program_event(next->dev, expires);
	}

	if (timer->callback)
		timer->callback(timer->context);
}

int hrtimer_init(struct hrtimer *timer)
{
	timer->state = HRTIMER_STATE_INACTIVE;
	timer->dev = clockevents_get_device("lm3s-timer0");
	clockevent_set_event_handler(timer->dev, hrtimer_interrupt);
	clockevents_switch_state(timer->dev, CLOCK_EVT_STATE_ONESHOT);

	return 0;
}

struct hrtimer *hrtimer_alloc(void)
{
	struct hrtimer *timer = kzalloc(sizeof(*timer));

	if (!timer)
		return NULL;
	hrtimer_init(timer);

	return timer;
}

static void nanosleep_callback(void *task)
{
	sched_enqueue(task);
	sched_enqueue(current);
	schedule();
}

static int hrtimer_nanosleep(const struct timespec *req)
{
	struct hrtimer timer;

	if (hrtimer_init(&timer))
		return -1;
	timer.callback = nanosleep_callback;
	timer.context = current;
	if (hrtimer_set_expires(&timer, timespec_to_ktime(*req)))
		return -1;
	current->state = TASK_UNINTERRUPTIBLE;
	schedule();

	return 0;
}

SYSCALL_DEFINE(nanosleep,
	const struct timespec	*req,
	struct timespec		*rem)
{
	(void)rem;

	return hrtimer_nanosleep(req);
}
