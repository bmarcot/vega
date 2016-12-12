/*
 * platform/nrf52/init.c
 *
 * Copyright (c) 2016 Benoit Marcot
 */

#include <sys/cdefs.h>

#include "platform.h"

struct timer_operations;

void config_timer_operations(struct timer_operations *tops);

extern struct timer_operations nrf52_tops;

__weak void __platform_init(void)
{
	config_timer_operations(&nrf52_tops);
}

__weak void __platform_halt(void)
{
	for (;;)
		;
}

void __printk_init(void)
{
	/* disable the module while (re)configuring */
	NRF_UARTE0->ENABLE = UARTE_ENABLE_ENABLE_Disabled << UARTE_ENABLE_ENABLE_Pos;

	/* 115200bps, no flow-control, no parity */
	NRF_UARTE0->CONFIG = (UARTE_CONFIG_HWFC_Disabled   << UARTE_CONFIG_HWFC_Pos)
		| (UARTE_CONFIG_PARITY_Excluded << UARTE_CONFIG_PARITY_Pos);
	NRF_UARTE0->BAUDRATE = UARTE_BAUDRATE_BAUDRATE_Baud115200 << UARTE_BAUDRATE_BAUDRATE_Pos;

	/* use nRF52 dev kit's default pins for UART function */
	NRF_UARTE0->PSEL.TXD = 6;
	NRF_UARTE0->PSEL.RXD = 8;

	/* enable data tx and rx */
	NRF_UARTE0->ENABLE = UARTE_ENABLE_ENABLE_Enabled << UARTE_ENABLE_ENABLE_Pos;
}

void __printk_putchar(char c)
{
	NRF_UARTE0->TXD.MAXCNT = 1;
	NRF_UARTE0->TXD.PTR = (uint32_t)&c;
	NRF_UARTE0->TASKS_STARTTX = 1;
	while (!NRF_UARTE0->EVENTS_ENDTX)
		;
	NRF_UARTE0->TASKS_STOPTX = 1;
	while (!NRF_UARTE0->EVENTS_TXSTOPPED)
		;
}