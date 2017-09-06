/*
 * kernel/time/clocksource.c
 *
 * Copyright (c) 2017 Baruch Marcot
 */

#include <kernel/time/clocksource.h>

static struct clocksource *clksrc;

int clocksource_register(struct clocksource *cs)
{
	clksrc = cs;

	return 0;
}

ktime_t clocksource_read(void)
{
	return clksrc->read(clksrc);
}
