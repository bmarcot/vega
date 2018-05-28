/*
 * include/kernel/sched_clock.h
 *
 * Copyright (c) 2018 Benoit Marcot
 */

#ifndef _KERNEL_SCHED_CLOCK_H
#define _KERNEL_SCHED_CLOCK_H

#include <kernel/types.h>

#include <asm/ktime.h>

void sched_clock_register(u64 (*read)(void), u32 mult, u32 shift);
ktime_t sched_clock(void);

#endif /* !_KERNEL_SCHED_CLOCK_H */
