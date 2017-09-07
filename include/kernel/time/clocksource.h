/*
 * include/kernel/time/clocksource.h
 *
 * Copyright (c) 2017 Benoit Marcot
 */

#ifndef _KERNEL_TIME_CLOCKSOURCE_H
#define _KERNEL_TIME_CLOCKSOURCE_H

#include <asm/ktime.h>

struct clocksource {
	ktime_t	(*read) (struct clocksource *cs);
	void	(*suspend) (struct clocksource *cs);
	void	(*resume) (struct clocksource *cs);
};

int clocksource_register(struct clocksource *cs);
ktime_t clocksource_read(void);
void clocksource_resume(void);

#endif /* !_KERNEL_TIME_CLOCKSOURCE_H */
