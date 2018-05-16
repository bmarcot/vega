/*
 * include/kernel/clocksource.h
 *
 * Copyright (c) 2017-2018 Benoit Marcot
 */

#ifndef _KERNEL_CLOCKSOURCE_H
#define _KERNEL_CLOCKSOURCE_H

#include <kernel/types.h>

struct clocksource {
	u64	(*read) (struct clocksource *cs);

	u32			mult;
	u32			shift;
	const char		*name;
	struct list_head	list;

	int	(*enable) (struct clocksource *cs);
	void	(*disable) (struct clocksource *cs);
	void	(*suspend) (struct clocksource *cs);
	void	(*resume) (struct clocksource *cs);
};

static inline u64 clocksource_read(struct clocksource *cs)
{
	return cs->read(cs);
}

static inline int clocksource_enable(struct clocksource *cs)
{
	return cs->enable(cs);
}

static inline void clocksource_disable(struct clocksource *cs)
{
	cs->disable(cs);
}

static inline void clocksource_suspend(struct clocksource *cs)
{
	cs->suspend(cs);
}

static inline void clocksource_resume(struct clocksource *cs)
{
	cs->resume(cs);
}

static inline u64 clocksource_cyc2ns(u64 cycles, u32 mult, u32 shift)
{
	return (cycles * mult) >> shift;
}

#include <asm/ktime.h>
ktime_t clock_monotonic_read(void);
void clock_monotonic_resume(void);
int clock_monotonic_register(struct clocksource *clksrc);

#endif /* !_KERNEL_CLOCKSOURCE_H */
