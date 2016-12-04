/*
 * drivers/timer/nrf52.h
 *
 * Copyright (c) 2016 Benoit Marcot
 */

#ifndef _DRIVERS_TIMER_NRF52_H
#define _DRIVERS_TIMER_NRF52_H

#include <kernel/time.h>

#include "platform.h"

struct nrf52_timer_const {
	NRF_TIMER_Type *nrf_timer;
	int irq_no;
	void (*isr_entry)(void);
};

struct nrf52_timer {
	struct timer_info *timer; /* backlink to kernel struct */
};

void nrf52_timer0_isr_entry(void);
void nrf52_timer1_isr_entry(void);
void nrf52_timer2_isr_entry(void);
void nrf52_timer3_isr_entry(void);
void nrf52_timer4_isr_entry(void);

#endif /* !_DRIVERS_TIMER_NRF52_H */
