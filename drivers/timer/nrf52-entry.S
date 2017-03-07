/*
 * drivers/timer/nrf52-entry.c
 *
 * Copyright (c) 2016 Benoit Marcot
 */

#include <kernel/linkage.h>

ENTRY(nrf52_timer0_isr_entry)
	mov	r0, #0
	ldr	r1, =nrf52_timer_isr
	bx	r1
ENDPROC(nrf52_timer0_isr_entry)

ENTRY(nrf52_timer1_isr_entry)
	mov	r0, #1
	ldr	r1, =nrf52_timer_isr
	bx	r1
ENDPROC(nrf52_timer1_isr_entry)

ENTRY(nrf52_timer2_isr_entry)
	mov	r0, #2
	ldr	r1, =nrf52_timer_isr
	bx	r1
ENDPROC(nrf52_timer2_isr_entry)

ENTRY(nrf52_timer3_isr_entry)
	mov	r0, #3
	ldr	r1, =nrf52_timer_isr
	bx	r1
ENDPROC(nrf52_timer3_isr_entry)

ENTRY(nrf52_timer4_isr_entry)
	mov	r0, #4
	ldr	r1, =nrf52_timer_isr
	bx	r1
ENDPROC(nrf52_timer4_isr_entry)
