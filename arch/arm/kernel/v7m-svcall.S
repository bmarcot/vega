/*
 * arch/arm/kernel/v7m-svcall.S
 *
 * Copyright (c) 2016-2017 Benoit Marcot
 */

#include <kernel/linkage.h>

	.syntax unified
	.thumb

	/* offset from the cpu-saved frame */
	.set	C_R0,               0

	/* offset from the preserved registers frame */
	.set	U_R0,       	    0x20
	.set    U_RET_ADDR, 	    0x38
	.set	U_xPSR,     	    0x3c
	.set	U_LAST_ARG,	    0x40
	.set	U_LAST_ARG_UNALIGN, 0x44

ENTRY(svcall)
	push	{lr}			/* save LR on the kernel stack */
	mrs	lr, psp			/* save non-scratch regs on user stack */
	stmdb	lr!, {r4-r11}
	mov	r4, sp			/* save PSP in thread_info struct */
	bfc	r4, #0, #9
	str	lr, [r4, #4]
	ldr	r4, [lr, #U_RET_ADDR]	/* read memory at SVC call site */
	ldrb	r4, [r4, #-2]
	tbb	[pc, r4]

0:	.irpc	argc, 0123456
	.byte	($\argc - 0b) / 2
	.endr

	.balign	2
$0:	ldr	r4, =syscall_vect
	ldr.w	r4, [r4, r0, lsl #2]
	blx	r4
	b	0f

	.balign	2
$1:	ldr	r4, =syscall_vect
	ldr.w	r4, [r4, r1, lsl #2]
	blx	r4
	b	0f

	.balign	2
$2:	ldr	r4, =syscall_vect
	ldr.w	r4, [r4, r2, lsl #2]
	blx	r4
	b	0f

	.balign	2
$3:	ldr	r4, =syscall_vect
	ldr.w	r4, [r4, r3, lsl #2]
	blx	r4
	b	0f

	.balign	2
$4:	ldr	r4, [lr, #U_xPSR]	/* test stack alignment */
	tst	r4, #1 << 9
	ite	eq
	ldreq	lr, [lr, #U_LAST_ARG]	/* syscall id is last arg pushed to user stack */
	ldrne	lr, [lr, #U_LAST_ARG_UNALIGN]
	ldr	r4, =syscall_vect
	ldr.w	r4, [r4, lr, lsl #2]
	blx	r4
	b	0f

	.balign	2
$5:	ldr	r12, [lr, #U_xPSR]	@ test stack alignment
	tst	r12, #1 << 9
	ite	eq
	addeq	lr, #U_LAST_ARG
	addne	lr, #U_LAST_ARG_UNALIGN
	ldm	lr, {r12, lr}		@ load syscall id, arg4
	push	{r12}			@ copy arg4 to kernel stack
	ldr	r12, =syscall_vect
	ldr.w	r12, [r12, lr, lsl #2]
	blx	r12
	add	sp, #4
	b	0f

	.balign	2
$6:	ldr	r5, [lr, #U_xPSR]	@ test stack alignment
	tst	r5, #1 << 9
	ite	eq
	addeq	lr, #U_LAST_ARG
	addne	lr, #U_LAST_ARG_UNALIGN
	ldm	lr, {r4, r5, lr}	@ load syscall id, arg4, arg5
	push	{r4, r5}		@ copy arg4, arg5 to kernel stack
	ldr	r5, =syscall_vect
	ldr.w	r5, [r5, lr, lsl #2]
	blx	r5
	pop	{r4, r5}		//FIXME: pop {r4}; add	sp, #4;

	.global syscall_return
syscall_return:
0:	mov	r1, sp			/* restore PSP from thread_info struct */
	bfc	r1, #0, #9
	ldr	r1, [r1, #4]		//FIXME: offsetof thread_ctx->sp
	ldmia	r1!, {r4-r11}		/* restore non-scratch regs */
	msr	psp, r1
	str	r0, [r1, #C_R0]		/* update return value in exception frame */
	pop	{pc}
ENDPROC(svcall)
