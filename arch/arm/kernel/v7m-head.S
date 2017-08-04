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
	badr	r2, $0
	str	r2, [r1, #11 * 4]		/* offset to SVC entry */
	dsb					/* [1] */
	svc	#0
$0:

	@ restore the early vector
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
	movs	r1, #0
	ldr	r2, =__bss_size__
	bl	memset

	@ start_kernel procedure returns the first thread to run on the CPU
	bl	start_kernel
	tst	r0, r0
	itttt	ne
	movne	r1, #0
	movne	r2, #0
	ldrne	lr, =__switch_to
	bxne	lr
0:	b	0b
ENDPROC(reset_entry)

/* [1] http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.dai0321a/BIHDGBJG.html */

	.macro	int_enter
	mov	r0, sp			/* save psp value in thread_info struct */
	bfc	r0, #0, #9		//FIXME: ilog2(THREAD_SIZE)
	mrs	r1, psp
	stmdb	r1!, {r4-r11}		/* save non-scratch regs on user stack */
	str	r1, [r0, #4]		//FIXME: offsetof thread_ctx->sp
	.endm

	.macro	int_leave
	mov	r0, sp			/* restore psp */
	bfc	r0, #0, #9
	ldr	r0, [r0, #4]		//FIXME: offsetof thread_ctx->sp
	ldmia	r0!, {r4-r11}		/* restore non-scratch regs */
	msr	psp, r0
	ldr	lr, =0xfffffffd
	bx	lr
	.endm

ENTRY(irq_entry)
	int_enter
	ldr	r0, =irq_handler
	mrs	r1, ipsr
	sub	r1, #16
	ldr.w	r0, [r0, r1, lsl #2]
	blx	r0
	int_leave
ENDPROC(irq_entry)

ENTRY(systick_entry)
	int_enter
	bl	systick
	int_leave
ENDPROC(systick_entry)

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
	.long	svcall				@ SVCall
	.long	0				@ Debug Monitor
	.long	0				@ Reserved
	.long	0				@ PendSV
	.long	systick_entry			@ SysTick
	.rept	64
	.long	irq_entry			@ IRQ 0..63
	.endr
