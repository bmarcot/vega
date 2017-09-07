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

void lm3s6965_init(void);
void lm3s_timer_init(void);

__weak void __platform_init(void)
{
	/* create /dev/ttyS0, serial interface for Qemu UART0 */
	lm3s6965_init();
	init_systick(); /* SysTick as clocksource */
	clocksource_resume();

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
