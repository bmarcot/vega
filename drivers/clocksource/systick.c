/*
 * drivers/clocksource/systick.c
 *
 * Copyright (c) 2017 Benoit Marcot
 */

#include <kernel/time/clocksource.h>
#include <kernel/types.h>

#include <asm/ktime.h>

#include "platform.h"

static u64 ticks;

void systick(void)
{
	ticks++;
}

ktime_t systick_read(struct clocksource *cs)
{
#define NSEC_PER_SEC 1000000000l
	return ticks * ((u64)NSEC_PER_SEC / (u64)cs->freq_hz);
}

void systick_suspend(struct clocksource *cs)
{
	(void)cs;

	SysTick->CTRL = 0;
}

void systick_resume(struct clocksource *cs)
{
	SysTick->LOAD = (SystemFrequency / cs->freq_hz) - 1;
	SysTick->VAL = 0;
	SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk
		| SysTick_CTRL_TICKINT_Msk
		| SysTick_CTRL_ENABLE_Msk;
}

static struct clocksource clocksource_systick = {
	.read = systick_read,
	.suspend = systick_suspend,
	.resume = systick_resume,
	.freq_hz = 1000,
	.name = "systick-clock",
};

void init_systick(void)
{
	clock_monotonic_register(&clocksource_systick);
}
