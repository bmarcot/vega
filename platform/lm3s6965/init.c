/*
 * platform/lm3s6965/init.c
 *
 * Copyright (c) 2016 Benoit Marcot
 */

#include <kernel/compiler.h>
#include <kernel/kernel.h>

#include "platform.h"

#define SYSTICK_FREQ_IN_HZ 1000
#define SYSTICK_PERIOD_IN_MSECS (SYSTICK_FREQ_IN_HZ / 1000)

struct timer_operations;

void config_timer_operations(struct timer_operations *tops);

extern struct timer_operations systick_tops;

void lm3s6965_init(void);
void lm3s_timer_init(void);

__weak void __platform_init(void)
{
	config_timer_operations(&systick_tops);

	/* SysTick running at 1kHz */
	printk("Processor speed: %3d MHz\n", SystemFrequency / 1000000);
	printk("Timer precision: %3d msec\n", SYSTICK_PERIOD_IN_MSECS);
	SysTick_Config(SystemFrequency / SYSTICK_FREQ_IN_HZ);

	/* create /dev/ttyS0, serial interface for Qemu UART0 */
	lm3s6965_init();
	init_systick(); /* SysTick as clocksource */

	/* initialize a clock event device (Timer0A) */
	lm3s_timer_init();
}

__weak void __platform_halt(void)
{
	for (;;)
		;
}

void __printk_init(void)
{
	UART0->CTL |= 1; /* UART enabled */
	UART0->LCRH |= (3 << 5); /* 8 bits word length, no parity */
}

void __printk_putchar(char c)
{
	while (UART0->FR & (1 << 3))
		;
	UART0->DR = c;
}
