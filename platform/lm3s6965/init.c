/*
 * platform/lm3s6965/init.c
 *
 * Copyright (c) 2016 Benoit Marcot
 */

#include <sys/cdefs.h>

#include "kernel.h"

#include "platform.h"

#define SYSTICK_FREQ_IN_HZ 1000
#define SYSTICK_PERIOD_IN_MSECS (SYSTICK_FREQ_IN_HZ / 1000)

struct timer_operations;

void config_timer_operations(struct timer_operations *tops);

extern struct timer_operations systick_tops;

__weak void __platform_init(void)
{
	config_timer_operations(&systick_tops);

	/* SysTick running at 1kHz */
	printk("Processor speed: %3d MHz\n", SystemFrequency / 1000000);
	printk("Timer precision: %3d msec\n", SYSTICK_PERIOD_IN_MSECS);
	SysTick_Config(SystemFrequency / SYSTICK_FREQ_IN_HZ);
}

__weak void __platform_halt(void)
{
	for (;;)
		;
}
