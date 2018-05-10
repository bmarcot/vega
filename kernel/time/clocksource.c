/*
 * kernel/time/clocksource.c
 *
 * Copyright (c) 2017-2018 Benoit Marcot
 */

#include <kernel/clocksource.h>
#include <kernel/list.h>

static LIST_HEAD(clocks);

static struct clocksource *monotonic_clk;

void clock_monotonic_resume(void)
{
	return clocksource_resume(monotonic_clk);
}

ktime_t clock_monotonic_read(void)
{
	return clocksource_cyc2ns(clocksource_read(monotonic_clk),
				monotonic_clk->mult, monotonic_clk->shift);
}

int clock_monotonic_register(struct clocksource *clksrc)
{
	if (clksrc) {
		list_add(&clksrc->list, &clocks);
		monotonic_clk = clksrc;
	}

	return 0;
}
