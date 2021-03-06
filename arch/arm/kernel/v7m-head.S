/*
 * arch/arm/kernel/v7m-head.S
 *
 * Copyright (c) 2016-2017 Benoit Marcot
 */

#include <kernel/linkage.h>

#include <asm/assembler.h>

	.syntax unified
	.thumb

	.text

ENTRY(reset_entry)
	ldr	r0, =SystemInit			/* CMSIS system init */
	blx	r0

	mov32	r0, 0xe000ed00			/* SCB_BASE */

	@ switch to Handler_Mode
	ldr	r1, =__early_stack_end__	/* allocate a temporary vector */
	str	r1, [r0, #8]			/* update VTOR */
	badr	r2, 0f
	str	r2, [r1, #11 * 4]		/* offset to SVC entry */
	dsb					/* [1] */
	svc	#0
0:

	@ restore the exception vector
	ldr	r1, =__vector_start__
	str	r1, [r0, #8]			/* restore VTOR */
	dsb					/* [1] */

	@ copy the initialized data sections
	ldr	r0, =__data_start__
	ldr	r1, =__rodata_end__
	ldr	r2, =__data_size__
	bl	memcpy

	@ zero-fill the non-initialized data sections
	ldr	r0, =__bss_start__
	mov	r1, #0
	ldr	r2, =__bss_size__
	bl	memset

	@ start_kernel procedure returns the first thread to run on the CPU
	bl	start_kernel
	tst	r0, r0
	itttt	ne
	movne	r1, #0
	movne	r2, #0
	ldrne	lr, =resume
	bxne	lr
0:	b	0b
ENDPROC(reset_entry)

/* [1] http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.dai0321a/BIHDGBJG.html */

	.section ".vector", "a"

	.long	__early_stack_start__		@ SP_main value
	.long	reset_entry			@ Reset
	.long	0				@ NMI
	.long	hardf				@ HardFault
	.long	memf				@ MemManage
	.long	busf				@ BusFault
	.long	usgf				@ UsageFault
	.long	0				@ Reserved
	.long	0				@ Reserved
	.long	0				@ Reserved
	.long	0				@ Reserved
	.long	svcall_entry			@ SVCall
	.long	0				@ Debug Monitor
	.long	0				@ Reserved
	.long	0				@ PendSV
	.long	systick_entry			@ SysTick
	.rept	64
	.long	irq_entry			@ IRQ 0..63
	.endr
