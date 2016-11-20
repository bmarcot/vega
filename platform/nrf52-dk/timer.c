/*
 * ../nrf52/timer.c
 *
 * Copyright (C) 2016 Benoit Marcot
 */

#include <kernel/bitops.h>
#include <kernel/irq.h>
#include <kernel/types.h>

#include "kernel.h"

#include "nrf.h"
#include <stddef.h>

#include "timer.h"

void timer0_isr_entry(void);
void timer1_isr_entry(void);
void timer2_isr_entry(void);
void timer3_isr_entry(void);
void timer4_isr_entry(void);

static const struct nrf52_timer_info timer_infos[] = {
	{NRF_TIMER0, TIMER0_IRQn, timer0_isr_entry},
	{NRF_TIMER1, TIMER1_IRQn, timer1_isr_entry},
	{NRF_TIMER2, TIMER2_IRQn, timer2_isr_entry},
	{NRF_TIMER3, TIMER3_IRQn, timer3_isr_entry},
	{NRF_TIMER4, TIMER4_IRQn, timer4_isr_entry},
};

static unsigned long timer_bitmap;

static struct nrf52_timer timers[NRF52_TIMERS];

struct nrf52_timer *alloc_timer(void)
{
	unsigned long bit = find_first_zero_bit(&timer_bitmap, NRF52_TIMERS);
	if (bit == NRF52_TIMERS)
		return NULL;
	set_bit(bit, &timer_bitmap);

	return &timers[bit];
}

void configure_timer(struct nrf52_timer *timer, void (*isr_callback)(int))
{
	unsigned int i = ARRAY_INDEX(timer, timers);
	unsigned int irq_no = timer_infos[i].irq_no;
	if (isr_callback) {
		timer->isr_callback = isr_callback;
		timer_infos[i].nrf_timer->INTENSET =
			TIMER_INTENSET_COMPARE0_Set << TIMER_INTENSET_COMPARE0_Pos;
		NVIC_EnableIRQ(irq_no);
	} else {
		timer_infos[i].nrf_timer->INTENCLR =
			TIMER_INTENCLR_COMPARE0_Clear << TIMER_INTENCLR_COMPARE0_Pos;
		NVIC_DisableIRQ(irq_no);
	}
}

void set_timer(struct nrf52_timer *timer, unsigned int usec)
{
	/* lookup tables */
	const u8 prescalers[] = {9, 4, 5, 4, 6, 4, 5, 4,
				 7, 4, 5, 4, 6, 4, 5, 4,
				 8, 4, 5, 4, 6, 4, 5, 4,
				 7, 4, 5, 4, 6, 4, 5, 4};
	const u8 usec_per_tick[] = {1, 2, 4, 8, 16, 32};
	const u8 bitmodes[] = {TIMER_BITMODE_BITMODE_32Bit,
			       TIMER_BITMODE_BITMODE_24Bit,
			       TIMER_BITMODE_BITMODE_16Bit,
			       TIMER_BITMODE_BITMODE_08Bit};

	/* compute the minimum number of comparisons */
	u8 prescaler = prescalers[usec % 32];
	unsigned int cc = usec / usec_per_tick[prescaler - 4];
	u8 bitmode = bitmodes[__builtin_clz(cc) / 4];

	/* setup the peripheral */
	unsigned int i = ARRAY_INDEX(timer, timers);
	timer_infos[i].nrf_timer->CC[0] = cc;
	timer_infos[i].nrf_timer->BITMODE = bitmode << TIMER_BITMODE_BITMODE_Pos;
	timer_infos[i].nrf_timer->PRESCALER =
		prescaler << TIMER_PRESCALER_PRESCALER_Pos;
	timer_infos[i].nrf_timer->TASKS_START = 1;
}

void cancel_timer()
{
}

void free_timer(struct nrf52_timer *timer)
{
	if (timer->running == 1)
		cancel_timer(timer);
	unsigned long bit = ARRAY_INDEX(timer, timers);
	clear_bit(bit, &timer_bitmap);
}

void timer_isr(int timerid)
{
	timer_infos[timerid].nrf_timer->EVENTS_COMPARE[0] = 0; /* acknowledge */
	timers[timerid].isr_callback(timerid);
}

void timer_init(void)
{
	for (int i; i < NRF52_TIMERS; i++) {
		irq_attach(timer_infos[i].irq_no, timer_infos[i].isr_entry);
		timer_infos[i].nrf_timer->MODE =
			TIMER_MODE_MODE_Timer << TIMER_MODE_MODE_Pos;
		timer_infos[i].nrf_timer->SHORTS =
			(TIMER_SHORTS_COMPARE0_CLEAR_Enabled << TIMER_SHORTS_COMPARE0_CLEAR_Pos)
			| (TIMER_SHORTS_COMPARE0_STOP_Enabled << TIMER_SHORTS_COMPARE0_STOP_Pos);
	}
}


void sys_timer_settime()
{
}
