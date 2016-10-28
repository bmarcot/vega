/*
 * v7m-svcall.S
 *
 * Copyright (c) 2016 Benoit Marcot
 */

	.syntax unified
	.thumb

#include "linux/linkage.h"

	@ offsets in the frame to the registers saved on interrupt-entry
	.set	R0, 0
	.set    RET_ADDRESS, 24
	.set	XPSR, 0x1c

	.global syscall_return

ENTRY(svcall)
	push	{lr}
	mrs	r12, psp
	ldr	lr, [r12, #RET_ADDRESS]
	ldrb	lr, [lr, #-2]           @ address of the SVC call site
	tbb	[pc, lr]

0:	.irpc	argc, 012345
	.byte	($\argc - 0b) / 2
	.endr
	.balign 2

$0:	ldr	lr, =syscall_vector
	ldr.w	lr, [lr, r0, lsl #2]
	blx	lr
	b	0f

$1:	ldr	lr, =syscall_vector
	ldr.w	lr, [lr, r1, lsl #2]
	blx	lr
	b	0f

$2:	ldr	lr, =syscall_vector
	ldr.w	lr, [lr, r2, lsl #2]
	blx	lr
	b	0f

$3:	ldr	lr, =syscall_vector
	ldr.w	lr, [lr, r3, lsl #2]
	blx	lr
	b	0f

$4:	ldr	lr, [r12, #0x1c]	@ is stack 4 or 8-byte aligned?
	tst	lr, #1 << 9
	ite	eq
	ldreq	r12, [r12, #0x20]	@ load syscall id
	ldrne	r12, [r12, #0x24]
	ldr	lr, =syscall_vector
	ldr.w	lr, [lr, r12, lsl #2]
	blx	lr
	b	0f

$5:	ldr	lr, [r12, #0x1c]	@ is stack 4 or 8-byte aligned?
	tst	lr, #1 << 9
	ite	eq
	addeq	r12, #0x20
	addne	r12, #0x24
	ldm	r12, {r12, lr}		@ load syscall id, arg4
	push	{lr}			@ copy arg4 to kernel stack
	ldr	lr, =syscall_vector
	ldr.w	lr, [lr, r12, lsl #2]
	blx	lr
	add	sp, #4

	// we dont care about the scratch registers at this point
syscall_return:
0:	mrs	r1, psp
	str	r0, [r1, #R0]           @ update return value in exception frame
	pop	{pc}
ENDPROC(svcall)
