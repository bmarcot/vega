/*
 * arch/arm/kernel/setup.c
 *
 * Copyright (c) 2017-2018 Benoit Marcot
 */

#include <kernel/clocksource.h>
#include <kernel/kernel.h>
#include <kernel/sched_clock.h>

#include "platform.h"

static struct clocksource cs_systick = {
	/* f = 12MHz: from = 12000000, to = 1000000000, maxsec = 1 */
	.mult = -1498764629,
	.shift = 25,

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

static u64 systick_sched_clock(void)
{
	return clocksource_read(&cs_systick);
}

void setup_arch(void)
{
	cpu_init();

	/* Configure the architectural SysTick as a clocksource */
	clocksource_init_systick(&cs_systick);
	clocksource_enable(&cs_systick);

	/* Register the SysTick as sched_clock() source */
	sched_clock_register(systick_sched_clock, cs_systick.mult, cs_systick.shift);
	printk("Registered %s as sched_clock source\n", cs_systick.name);
}
