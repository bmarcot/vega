/*
 * drivers/timer/nrf52.c
 *
 * Copyright (c) 2016 Benoit Marcot
 */

#include <errno.h>
#include <stddef.h>
#include <time.h>

#include <kernel/bitops.h>
#include <kernel/errno-base.h>
#include <kernel/irq.h>
#include <kernel/kernel.h>
#include <kernel/time.h>

#include "nrf52.h"
#include "platform.h"

static const struct nrf52_timer_const nrf52_timer_consts[] = {
	{NRF_TIMER0, TIMER0_IRQn, nrf52_timer0_isr_entry},
	{NRF_TIMER1, TIMER1_IRQn, nrf52_timer1_isr_entry},
	{NRF_TIMER2, TIMER2_IRQn, nrf52_timer2_isr_entry},
	{NRF_TIMER3, TIMER3_IRQn, nrf52_timer3_isr_entry},
	{NRF_TIMER4, TIMER4_IRQn, nrf52_timer4_isr_entry},
};

static struct nrf52_timer nrf52_timers[TIMER_COUNT];

static unsigned long timer_bitmap;

int nrf52_timer_alloc(struct timer_info *timer)
{
	unsigned long bit = find_first_zero_bit(&timer_bitmap, TIMER_COUNT);

	if (bit == TIMER_COUNT) {
		errno = EAGAIN;
		return -1;
	}
	set_bit(bit, &timer_bitmap);
	timer->dev = &nrf52_timers[bit];
	nrf52_timers[bit].timer = timer;

	return 0;
}

int nrf52_timer_configure(struct timer_info *timer,
			void (*callback)(struct timer_info *self))
{
	unsigned int i = ARRAY_INDEX(timer->dev, nrf52_timers);

	if (callback) {
		nrf52_timer_consts[i].nrf_timer->INTENSET =
			TIMER_INTENSET_COMPARE0_Set << TIMER_INTENSET_COMPARE0_Pos;
		NVIC_EnableIRQ(nrf52_timer_consts[i].irq_no);
	} else {
		nrf52_timer_consts[i].nrf_timer->INTENCLR =
			TIMER_INTENCLR_COMPARE0_Clear << TIMER_INTENCLR_COMPARE0_Pos;
		NVIC_DisableIRQ(nrf52_timer_consts[i].irq_no);
	}

	return 0;
}

int nrf52_timer_set(struct timer_info *timer, const struct itimerspec *new_value)
{
	struct nrf52_timer *nrf52_timer = timer->dev;

	unsigned long usecs = new_value->it_value.tv_sec * 1000000
		+ new_value->it_value.tv_nsec / 1000;

	/* lookup tables */
	const u8 prescalers[] = {9, 4, 5, 4, 6, 4, 5, 4,
				 7, 4, 5, 4, 6, 4, 5, 4,
				 8, 4, 5, 4, 6, 4, 5, 4,
				 7, 4, 5, 4, 6, 4, 5, 4};
	const u8 usecs_per_tick[] = {1, 2, 4, 8, 16, 32};
	const u8 bitmodes[] = {TIMER_BITMODE_BITMODE_32Bit,
			       TIMER_BITMODE_BITMODE_24Bit,
			       TIMER_BITMODE_BITMODE_16Bit,
			       TIMER_BITMODE_BITMODE_08Bit};

	/* compute the minimum number of comparisons */
	u8 prescaler = prescalers[usecs % 32];
	unsigned int cc = usecs / usecs_per_tick[prescaler - 4];
	u8 bitmode = bitmodes[__builtin_clz(cc) / 8];

	/* setup the peripheral */
	unsigned int i = ARRAY_INDEX(nrf52_timer, nrf52_timers);
	nrf52_timer_consts[i].nrf_timer->CC[0] = cc;
	nrf52_timer_consts[i].nrf_timer->BITMODE = bitmode << TIMER_BITMODE_BITMODE_Pos;
	nrf52_timer_consts[i].nrf_timer->PRESCALER =
		prescaler << TIMER_PRESCALER_PRESCALER_Pos;
	nrf52_timer_consts[i].nrf_timer->TASKS_START = 1;

	return 0;
}


int nrf52_timer_cancel(struct timer_info *timer)
{
	(void)timer;

	return 0;
}

int nrf52_timer_free(struct timer_info *timer)
{
	(void)timer;

	return 0;
}

void nrf52_timer_isr(int timerid)
{
	nrf52_timer_consts[timerid].nrf_timer->EVENTS_COMPARE[0] = 0;
	timer_expire_callback(nrf52_timers[timerid].timer);
}

void nrf52_timer_init(void)
{
	for (int i; i < TIMER_COUNT; i++) {
		irq_attach(nrf52_timer_consts[i].irq_no, nrf52_timer_consts[i].isr_entry);
		nrf52_timer_consts[i].nrf_timer->MODE =
			TIMER_MODE_MODE_Timer << TIMER_MODE_MODE_Pos;
		nrf52_timer_consts[i].nrf_timer->SHORTS =
			(TIMER_SHORTS_COMPARE0_CLEAR_Enabled << TIMER_SHORTS_COMPARE0_CLEAR_Pos)
			| (TIMER_SHORTS_COMPARE0_STOP_Enabled << TIMER_SHORTS_COMPARE0_STOP_Pos);
	}
}

struct timer_operations nrf52_tops = {
	.timer_alloc = nrf52_timer_alloc,
	.timer_configure = nrf52_timer_configure,
	.timer_set = nrf52_timer_set,
	.timer_cancel = nrf52_timer_cancel,
	.timer_free = nrf52_timer_free,
};
