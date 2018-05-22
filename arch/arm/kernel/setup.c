/*
 * arch/arm/kernel/setup.c
 *
 * Copyright (c) 2017-2018 Benoit Marcot
 */

#include <kernel/clocksource.h>
#include <kernel/kernel.h>
#include <kernel/sched_clock.h>

#include "platform.h"

static struct clocksource clocksource_systick = {
	.mult = 174762667,
	.shift = 21,
	.name = "systick",
};

void clocksource_init_systick(struct clocksource *cs);

void cpu_init(void)
{
	/* Enable additional fault handlers */
	SCB->SHCSR |= SCB_SHCSR_USGFAULTENA_Msk;
	SCB->SHCSR |= SCB_SHCSR_BUSFAULTENA_Msk;
	SCB->SHCSR |= SCB_SHCSR_MEMFAULTENA_Msk;

	/* Use 8-byte stack alignment on exception entry */
	SCB->CCR |= SCB_CCR_STKALIGN_Msk;

	/* Architecturally required */
	__DSB();
}

void setup_arch(void)
{
	cpu_init();

	/* Register SysTick as sched_clock source */
	clocksource_init_systick(&clocksource_systick);
	clocksource_enable(&clocksource_systick);
	register_sched_clock(&clocksource_systick);
}
