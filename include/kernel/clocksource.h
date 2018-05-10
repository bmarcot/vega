/*
 * include/kernel/clocksource.h
 *
 * Copyright (c) 2017-2018 Benoit Marcot
 */

#ifndef _KERNEL_CLOCKSOURCE_H
#define _KERNEL_CLOCKSOURCE_H

#include <kernel/types.h>

#include <asm/ktime.h>

struct clocksource {
	ktime_t	(*read) (struct clocksource *cs);
	void	(*suspend) (struct clocksource *cs);
	void	(*resume) (struct clocksource *cs);

	unsigned int		freq_hz;
	const char		*name;
	struct list_head	list;
};

static inline ktime_t clocksource_read(struct clocksource *clksrc)
{
	return clksrc->read(clksrc);
}

static inline void clocksource_resume(struct clocksource *clksrc)
{
	return clksrc->resume(clksrc);
}

ktime_t clock_monotonic_read(void);
void clock_monotonic_resume(void);
int clock_monotonic_register(struct clocksource *clksrc);

#endif /* !_KERNEL_CLOCKSOURCE_H */
