/*
 * kernel/time/hrtimer.c
 *
 * Copyright (c) 2017 Benoit Marcot
 */

#include <kernel/list.h>
#include <kernel/time/clockevents.h>
#include <kernel/time/hrtimer.h>

#include <asm/ktime.h>

static ktime_t hrtimer_clk; // clock is not monotonic, and can be suspended when no timers are running

LIST_HEAD(hrtimers);

int __hrtimer_insert_empty_list(struct hrtimer *timer, ktime_t expires)
{
	expires += hrtimer_clk;
	timer->expires = expires;
	list_add(&timer->list, &timers);

	return clockevents_program_event(timer->dev, expires);
}

int __hrtimer_insert_list(struct hrtimer *timer, ktime_t expires)
{
	struct timer *t;

	hrtimer_clk += clockevents_read_elapsed(timer->dev);
	expires += hrtimer_clk;
	timer->expires = expires;

	list_for_each_entry(t, &hrtimers, list) {
		if (t->expires > expires) {
			list_add_tail(&timer->list, &t->list);
			if (list_is_first(&timer->list, &hrtimers))
				return clockevents_program_event(timer->dev, expires);
			return 0;
		}
	}
	list_add_tail(&timer->list, &timers);

	return 0;
}

void hrtimer_event_handler(struct clock_event_device *)
{
	struct hrtimer *timer;

	timer = list_first_entry_or_null(&hrtimers, struct hrtimer, list);
	if (!timer) {
		pr_err("Got interrupt, but no timers are registered");
		return; //XXX: Something went wrong
	}
	hrtimer_clk = timer->expires;
	list_del(&timer->list);

	timer->callback(timer->context);
}

int htimer_new(struct hrtimer *timer/* , ktime_t expires */)
{
	//timer->dev = clockevents_get_device();
	clockevents_set_event_handler(timer->dev, hrtimer_event_handler);

	//XXX: suspend the clock_event_device while we insert?

	if (list_is_empty(&timers))
		__hrtimer_insert_empty_list(timer, expires);
	else
		__hrtimer_insert_list(timer, expires);

	//XXX: resume the clock_event_device
}

#define NSEC_PER_SEC 1000000000l

static void callback(void *nothing)
{
	(void)nothing;
	pr_info("In htrimer callback");
}

void hrtimer_test(void)
{
	struct hrtimer timer = {
		.expires = 2 * NSEC_PER_SEC,
		.dev = clockevt_dev,
		.callback = callback,
	};

	hrtimer_new(&timer);
}
