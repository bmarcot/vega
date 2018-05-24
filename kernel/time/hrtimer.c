/*
 * kernel/time/hrtimer.c
 *
 * Copyright (c) 2017-2018 Benoit Marcot
 */

#include <kernel/clockevents.h>
#include <kernel/errno-base.h>
#include <kernel/hrtimer.h>
#include <kernel/ktime.h>
#include <kernel/list.h>
#include <kernel/time.h>

#include <asm/ktime.h>

static LIST_HEAD(hrtimers);

static ktime_t hrtimer_get_next_event(void) //XXX: Param should be `clockid`
{
	struct hrtimer *timer = list_first_entry(&hrtimers, struct hrtimer, list);

	return timer->expires;
}

static void hrtimer_reprogram(void)
{
	//XXX: Refactor this function with the introduction of `clockid`

	if (list_empty(&hrtimers)) {
		struct clock_event_device *ced = clockevents_get_device(HRTIMER_DEVICE);
		clockevents_shutdown(ced);
	} else {
		ktime_t expires = hrtimer_get_next_event();
		struct hrtimer *timer = list_first_entry(&hrtimers, struct hrtimer, list);
		clockevents_program_event(timer->dev, expires);
	}
}

/*
 * Adds the timer node to the timerqueue, sorted by the node's expires
 * value. Returns true if the newly added timer is the first expiring timer
 * in the queue.
 */
static bool enqueue_hrtimer(struct hrtimer *timer)
{
	timer->state = HRTIMER_STATE_ENQUEUED;

	/* Timer queue is empty */
	if (list_empty(&hrtimers)) {
		list_add(&timer->list, &hrtimers);

		return true;
	}

	/* Timer queue is not empty, and timer expires first */
	struct hrtimer *ht = list_first_entry(&hrtimers, struct hrtimer, list);
	if (timer->expires < ht->expires) {
		list_add(&timer->list, &hrtimers);

		return true;
	}

	/* Insert new timer in the middle of the timer queue */
	list_for_each_entry(ht, &hrtimers, list) {
		if (timer->expires < ht->expires) {
			list_add_tail(&timer->list, &ht->list);

			return false;
		}
	}

	/* Timer will expire last */
	list_add_tail(&timer->list, &hrtimers);

	return false;
}

static bool remove_hrtimer(struct hrtimer *timer)
{
	bool reprogram = false;

	if (timer->state == HRTIMER_STATE_ENQUEUED) {
		timer->state = HRTIMER_STATE_INACTIVE;
		if (list_is_first(&timer->list, &hrtimers))
			reprogram = true;
		list_del(&timer->list);
	}

	return reprogram;
}


int hrtimer_start(struct hrtimer *timer, ktime_t expires /* , const enum hrtimer_mode mode */)
{
	//XXX: Pause clockevent device during insertion?

	//XXX: Do nothing or trigger event?
	if (!expires)
		return 0;

	timer->expires = expires + clockevents_read_elapsed(timer->dev);

	if (enqueue_hrtimer(timer)) {
		hrtimer_reprogram();
		return 1;
	}

	return 0;
}

int hrtimer_cancel(struct hrtimer *timer)
{
	if (remove_hrtimer(timer))
		hrtimer_reprogram();

	return 0;
}

static void hrtimer_interrupt(struct clock_event_device *dev)
{
	struct hrtimer *timer;

	/* Timer queue cannot be empty */
	timer = list_first_entry(&hrtimers, struct hrtimer, list);

	list_del(&timer->list);
	timer->state = HRTIMER_STATE_INACTIVE;

	if (timer->function && (timer->function(timer) == HRTIMER_RESTART))
		enqueue_hrtimer(timer);

	if (!list_empty(&hrtimers))
		hrtimer_reprogram();
}

void hrtimer_init(struct hrtimer *timer)
{
	timer->state = HRTIMER_STATE_INACTIVE;
	timer->dev = clockevents_get_device(HRTIMER_DEVICE);
	clockevent_set_event_handler(timer->dev, hrtimer_interrupt);
}
