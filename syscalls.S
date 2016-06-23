
	.syntax unified
	.thumb

#include "linux/linkage.h"

	@ offsets in the frame to the registers saved on interrupt-entry
	.set	R0, 0
	.set	RET_ADDRESS, 24

ENTRY(svcall)
	mrs	r0, psp
	ldr	r1, [r0, #RET_ADDRESS]

#if __ARM_ARCH == 6 /* __ARM_ARCH_6M__ */
	subs	r1, #2
	ldrb	r1, [r1]		@ address of the SVC call site
	cmp	r1, #0
	beq	do_syscall0
	cmp	r1, #1
	beq	do_syscall1
	cmp	r1, #2
	beq	do_syscall2
	cmp	r1, #3
	beq	do_syscall3
	cmp	r1, #4
	beq	do_syscall4
#elif __ARM_ARCH == 7 /* __ARM_ARCH_7M__ || __ARM_ARCH_7EM__ */
	ldrb	r1, [r1, #-2]		@ address of the SVC call site
	tbb	[pc, r1]		@ only r0, r1 have been clobbered so far

0:	.irpc	argc, 01234
	.byte	(do_syscall\argc - 0b) / 2
	.endr
#endif
	
	.balign 2
do_syscall0:
	push	{lr}
	ldr	r0, [r0]		@ r0 -> syscall id
	ldr	r1, =sys_vect
#if __ARM_ARCH == 6 /* __ARM_ARCH_6M__ */
	lsls	r0, #2
	ldr	r1, [r1, r0]
#elif __ARM_ARCH == 7 /* __ARM_ARCH_7M__ || __ARM_ARCH_7EM__ */
	ldr.w	r1, [r1, r0, lsl #2]
#endif
	blx	r1
	b	1f

	.balign 2
do_syscall1:
	push	{lr}
	ldm	r0, {r0, r1}		@ r0 -> param, r1 -> syscall id
	ldr	r2, =sys_vect
#if __ARM_ARCH == 6 /* __ARM_ARCH_6M__ */
	lsls	r1, #2
	ldr	r2, [r2, r1]
#elif __ARM_ARCH == 7 /* __ARM_ARCH_7M__ || __ARM_ARCH_7EM__ */
	ldr.w	r2, [r2, r1, lsl #2]
#endif
	blx	r2
	b	1f

	.balign 2
do_syscall2:
	push	{lr}
	ldm	r0, {r0, r1}		@ r0, r1 -> params, r2 -> syscall id
	ldr	r3, =sys_vect
#if __ARM_ARCH == 6 /* __ARM_ARCH_6M__ */
	lsls	r2, #2
	ldr	r3, [r3, r2]
#elif __ARM_ARCH == 7 /* __ARM_ARCH_7M__ || __ARM_ARCH_7EM__ */
	ldr.w	r3, [r3, r2, lsl #2]
#endif
	blx	r3
	b	1f

	.balign 2
do_syscall3:
#if __ARM_ARCH == 6 /* __ARM_ARCH_6M__ */
	push	{r7, lr}
	ldm	r0, {r0, r1}		@ r0-r2 -> params, r3 -> syscall id
	ldr	r7, =sys_vect
	lsls	r3, #2
	ldr	r7, [r7, r3]
	blx	r7
	mrs	r1, psp
	str	r0, [r1, #R0]		@ store the return value in the frame
	pop	{r7, pc}
#elif __ARM_ARCH == 7 /* __ARM_ARCH_7M__ || __ARM_ARCH_7EM__ */
	push	{lr}
	ldm	r0, {r0, r1}		@ r0-r2 -> params, r3 -> syscall id
	ldr	r12, =sys_vect
	ldr.w	r12, [r12, r3, lsl #2]
	blx	r12
	b	1f //FIXME: not needed?
#endif

1:	mrs	r1, psp
	str	r0, [r1, #R0]		@ store the return value in the frame
	pop	{pc}
	
	.balign 2
do_syscall4:
#if __ARM_ARCH == 6 /* __ARM_ARCH_6M__ */
	push	{r6, r7, lr}
	ldm	r0, {r0, r1}		@ r0-r3 -> params, r12 -> syscall id
	mov	r6, r12
	lsls	r6, #2
	ldr	r7, =sys_vect
	ldr	r7, [r7, r6]
	blx	r7
	mrs	r1, psp
	str	r0, [r1, #R0]		@ store the return value in the frame
	pop	{r6, r7, pc}
#elif __ARM_ARCH == 7 /* __ARM_ARCH_7M__ || __ARM_ARCH_7EM__ */
	push	{r7, lr}
	ldm	r0, {r0, r1}		@ r0-r3 -> params, r12 -> syscall id
	ldr	r7, =sys_vect
	ldr.w	r7, [r7, r12, lsl #2]
	blx	r7
	mrs	r1, psp
	str	r0, [r1, #R0]		@ store the return value in the frame
	pop	{r7, pc}
#endif

ENDPROC(svcall)
