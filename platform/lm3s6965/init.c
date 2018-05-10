/*
 * platform/lm3s6965/init.c
 *
 * Copyright (c) 2016 Benoit Marcot
 */

#include <kernel/clocksource.h>
#include <kernel/compiler.h>
#include <kernel/kernel.h>

#include "platform.h"

void lm3s6965_init(void);
void lm3s_timer_init(void);
void clocksource_init_systick(struct clocksource *cs);

static struct clocksource cs_systick = {
	.mult = 125,
	.shift = 1,
	.name = "systick",
};

__weak void __platform_init(void)
{
	/* create /dev/ttyS0, serial interface for Qemu UART0 */
	lm3s6965_init();
	clocksource_init_systick(&cs_systick);
	clock_monotonic_register(&cs_systick);
	clock_monotonic_resume();

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
