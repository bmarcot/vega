/*
 * kernel/time/sched_clock.c
 *
 * Copyright (c) 2017-2018 Benoit Marcot
 */

#include <kernel/clocksource.h>
#include <kernel/ktime.h>

struct clock_read_data {
	u64	epoch_cyc;
	u64	(*read_sched_clock) (void);
	u32	mult;
	u32	shift;
};

static struct clock_read_data crd;

int sched_clock_registered;

void sched_clock_register(u64 (*read)(void), u32 mult, u32 shift)
{
	crd.epoch_cyc = read();
	crd.read_sched_clock = read;
	crd.mult = mult;
	crd.shift = shift;

	sched_clock_registered = 1;
}

ktime_t sched_clock(void)
{
	u64 cyc = crd.read_sched_clock() - crd.epoch_cyc;

	return clocksource_cyc2ns(cyc, crd.mult, crd.shift);
}
