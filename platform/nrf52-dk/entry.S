#include "linux/linkage.h"

ENTRY(timer0_isr_entry)
	mov	r0, #0
	b	timer_isr
ENDPROC(timer0_isr_entry)

ENTRY(timer1_isr_entry)
	mov	r0, #1
	b	timer_isr
ENDPROC(timer1_isr_entry)

ENTRY(timer2_isr_entry)
	mov	r0, #2
	b	timer_isr
ENDPROC(timer2_isr_entry)

ENTRY(timer3_isr_entry)
	mov	r0, #3
	b	timer_isr
ENDPROC(timer3_isr_entry)

ENTRY(timer4_isr_entry)
	mov	r0, #4
	b	timer_isr
ENDPROC(timer4_isr_entry)
