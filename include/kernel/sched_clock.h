/*
 * include/kernel/sched_clock.h
 *
 * Copyright (c) 2018 Benoit Marcot
 */

#ifndef _KERNEL_SCHED_CLOCK_H
#define _KERNEL_SCHED_CLOCK_H

#include <asm/ktime.h>

struct clocksource;

void register_sched_clock(struct clocksource *cs);
ktime_t sched_clock(void);

#endif /* !_KERNEL_SCHED_CLOCK_H */
