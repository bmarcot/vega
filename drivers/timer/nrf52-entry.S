/*
 * drivers/timer/nrf52-entry.c
 *
 * Copyright (c) 2016 Benoit Marcot
 */

#include <kernel/linkage.h>

ENTRY(nrf52_timer0_isr_entry)
	mov	r0, #0
	b	nrf52_timer_isr
ENDPROC(nrf52_timer0_isr_entry)

ENTRY(nrf52_timer1_isr_entry)
	mov	r0, #1
	b	nrf52_timer_isr
ENDPROC(nrf52_timer1_isr_entry)

ENTRY(nrf52_timer2_isr_entry)
	mov	r0, #2
	b	nrf52_timer_isr
ENDPROC(nrf52_timer2_isr_entry)

ENTRY(nrf52_timer3_isr_entry)
	mov	r0, #3
	b	nrf52_timer_isr
ENDPROC(nrf52_timer3_isr_entry)

ENTRY(nrf52_timer4_isr_entry)
	mov	r0, #4
	b	nrf52_timer_isr
ENDPROC(nrf52_timer4_isr_entry)
