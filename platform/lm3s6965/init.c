/*
 * platform/lm3s6965/init.c
 *
 * Copyright (c) 2016-2018 Benoit Marcot
 */

#include <kernel/clocksource.h>
#include <kernel/compiler.h>
#include <kernel/kernel.h>
#include <kernel/sched_clock.h>

#include "platform.h"

void lm3s6965_init(void);
void lm3s_timer_init(void);
void clocksource_init_systick(struct clocksource *cs);

static struct clocksource clocksource_systick = {
	.mult = 174762667,
	.shift = 21,
	.name = "systick",
};

__weak void __platform_init(void)
{
	/* create /dev/ttyS0, serial interface for Qemu UART0 */
	lm3s6965_init();

	/* Register SysTick as sched_clock source */
	clocksource_init_systick(&clocksource_systick);
	clocksource_enable(&clocksource_systick);
	register_sched_clock(&clocksource_systick);

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

int putchar(int c)
{
	while (UART0->FR & (1 << 3))
		;
	UART0->DR = c;

	return c;
}

int puts(const char *s)
{
	for (; *s != '\0'; s++)
		putchar(*s);

	return 0;
}
