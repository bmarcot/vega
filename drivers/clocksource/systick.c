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

#define SYSTICK_FREQ_IN_HZ 1000
#define SYSTICK_PERIOD_IN_MSECS (SYSTICK_FREQ_IN_HZ / 1000)

void systick(void)
{
	ticks++;
}

ktime_t systick_read(struct clocksource *cs)
{
	(void)cs;

#define NSEC_PER_MSEC 1000000l
	return ticks * (ktime_t)NSEC_PER_MSEC;
}

void systick_suspend(struct clocksource *cs)
{
	(void)cs;

	SysTick_Config(0);
}

void systick_resume(struct clocksource *cs)
{
	(void)cs;

	SysTick_Config(SystemFrequency / SYSTICK_FREQ_IN_HZ);
}

static struct clocksource clocksource_systick = {
	.read = systick_read,
	.suspend = systick_suspend,
	.resume = systick_resume,
};

void init_systick()
{
	clocksource_register(&clocksource_systick);
}

/* int systick_clocksource_init(void) */
/* { */
/* /\* 	exynos4_mct_frc_start(); *\/ */

/* /\* #if defined(CONFIG_ARM) *\/ */
/* /\* 	exynos4_delay_timer.read_current_timer = &exynos4_read_current_timer; *\/ */
/* /\* 	exynos4_delay_timer.freq = clk_rate; *\/ */
/* /\* 	register_current_timer_delay(&exynos4_delay_timer); *\/ */
/* /\* 	#endif *\/ */

/* /\* 	if (clocksource_register_hz(&mct_frc, clk_rate)) *\/ */
/* /\* 		panic("%s: can't register clocksource\n", mct_frc.name); *\/ */

/* 	sched_clock_register(exynos4_read_sched_clock, 32, clk_rate); */

/* 	return 0; */
/* } */
