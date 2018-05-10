/*
 * drivers/clocksource/systick.c
 *
 * Copyright (c) 2017-2018 Benoit Marcot
 */

#include <kernel/clocksource.h>
#include <kernel/types.h>

#include "platform.h"

static u64 cycle_count;

/*
 * The SysTick interrupt handler
 */
void systick(void)
{
	cycle_count += (0xffffff + 1);
}

#include <kernel/kernel.h>
static u64 systick_read(struct clocksource *cs)
{
	// check COUNTFLAG
//	pr_info("status=%x", SysTick->CTRL);
	return cycle_count + 0xffffff - SysTick->VAL;
}

static void systick_suspend(struct clocksource *cs)
{
	SysTick->CTRL = 0;
}

static void systick_resume(struct clocksource *cs)
{
	SysTick->LOAD = 0xffffff;
	SysTick->VAL = 0;
	SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk
		| SysTick_CTRL_TICKINT_Msk
		| SysTick_CTRL_ENABLE_Msk;
}

void clocksource_init_systick(struct clocksource *cs)
{
	cs->read = systick_read;
	cs->suspend = systick_suspend;
	cs->resume = systick_resume;
}
