/*
 * drivers/clocksource/systick.c
 *
 * Copyright (c) 2017-2018 Benoit Marcot
 */

#include <kernel/clocksource.h>
#include <kernel/types.h>

#include "platform.h"

static u64 cycle_count;

static inline void handle_wrap_around(void)
{
	cycle_count += (0xffffff + 1);
}

// void systick_handler(void)
void systick(void)
{
	handle_wrap_around();
}

u64 systick_read(struct clocksource *cs)
{
	u32 val;

read_val:
	val = SysTick->VAL;

	/* Counter wrapped-around during syscall entry and now */
	if (SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk) {
		handle_wrap_around();
		SCB->SHCSR |= SCB_ICSR_PENDSTCLR_Msk;
		goto read_val;
	}

	return cycle_count + 0xffffff - val;
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
