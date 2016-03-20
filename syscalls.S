
	.syntax unified
	.thumb

#include "linux/linkage.h"

	@ offsets in the frame to the registers saved on interrupt-entry
	.set	R0, 0
	.set	RET_ADDRESS, 24

ENTRY(svcall)
	mrs	r0, psp
	ldr	r1, [r0, #RET_ADDRESS]
	ldr	r1, [r1, #-2]		@ address of the SVC call site
	and	r1, #0xff
	tbb	[pc, r1]		@ only r0, r1 have been clobbered so far

0:	.irpc	argc, 01234
	.byte	(do_syscall\argc - 0b) / 2
	.endr
	
	.balign 2
do_syscall0:
	push	{lr}
	ldr	r0, [r0]		@ r0 -> syscall id
	ldr	r1, =sys_vect
	ldr.w	r1, [r1, r0, lsl #2]
	blx	r1
	b	1f

	.balign 2
do_syscall1:
	push	{lr}
	ldm	r0, {r0, r1}		@ r0 -> param, r1 -> syscall id
	ldr	r2, =sys_vect
	ldr.w	r2, [r2, r1, lsl #2]
	blx	r2
	b	1f

	.balign 2
do_syscall2:
	push	{lr}
	ldm	r0, {r0, r1}		@ r0, r1 -> params, r2 -> syscall id
	ldr	r3, =sys_vect
	ldr.w	r3, [r3, r2, lsl #2]
	blx	r3
	b	1f

	.balign 2
do_syscall3:
	push	{lr}
	ldm	r0, {r0, r1}		@ r0-r2 -> params, r3 -> syscall id
	ldr	r12, =sys_vect
	ldr.w	r12, [r12, r3, lsl #2]
	blx	r12
	b	1f

1:	mrs	r1, psp
	str	r0, [r1, #R0]		@ store the return value in the frame
	pop	{pc}
	
	.balign 2
do_syscall4:
	push	{r7, lr}
	ldm	r0, {r0, r1}		@ r0-r3 -> params, r12 -> syscall id
	ldr	r7, =sys_vect
	ldr.w	r7, [r7, r12, lsl #2]
	blx	r7
	mrs	r1, psp
	str	r0, [r1, #R0]		@ store the return value in the frame
	pop	{r7, pc}
ENDPROC(svcall)
