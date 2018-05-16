/*
 * kernel/time/sched_clock.c
 *
 * Copyright (c) 2017-2018 Benoit Marcot
 */

#include <kernel/clocksource.h>
#include <kernel/kernel.h>
#include <kernel/ktime.h>

static struct clocksource *sched_clocksource;
static u64 epoch_cyc;

void register_sched_clock(struct clocksource *cs)
{
	epoch_cyc = clocksource_read(cs);
	sched_clocksource = cs;

	pr_info("Registered %s as sched_clock source", cs->name);
}

ktime_t sched_clock(void)
{
	u64 cyc = clocksource_read(sched_clocksource) - epoch_cyc;

	return clocksource_cyc2ns(cyc, sched_clocksource->mult,
				sched_clocksource->shift);
}
