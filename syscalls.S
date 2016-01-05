
	.syntax unified
	.thumb

#include "linux/linkage.h"

	@ int sc_no_args(void)
	.macro	sc_no_params id
	mov	r0, #\id
	svc	#0
	.endm

	@ int sc_one_arg(void *arg0)
	.macro	sc_one_param id
	/* keep r0 untouched */
	mov	r1, #\id
	svc	#1
	.endm

	@ int sc_two_args(void *arg0, void *arg1)
	.macro	sc_two_args id
	/* keep r0, r1 untouched */
	mov	r2, #\id
	svc	#3
	.endm

	@ int sc_three_args(void *arg0, void *arg1,
	@                   void *arg2)
	.macro	syscall3 id
	/* keep r0, r1, r2 untouched */
	mov	r3, #\id
	svc	#3
	.endm

	@ int sc_four_args(void *arg0, void *arg1,
	@                  void *arg2, void *arg3)
	.macro	syscall4 id
	/* keep r0, r1, r2, r3 untouched */
	mov	r12, #\id
	svc	#4
	.endm

	.set RET_ADDRESS, 24

ENTRY(svcall)
	mrs	r0, psp
	ldr	r1, [r0, #RET_ADDRESS]
	ldr	r1, [r1, #-2]		// address of the SVC instruction
	and	r1, r1, #0xff
	tbb	[pc, r1]

0:	.irpc	argc, 0123
	.byte	(do_syscall\argc - 0b) / 2
	.endr

	.set	R0, 0
	
	.balign 2
do_syscall0:
	push	{lr}
	ldr	r0, [r0, #R0] //syscall id
	ldr	r1, =sys_vect
	ldr.w	r1, [r1, r0, lsl #2]
	blx	r1
	b	1f

	.balign 2
do_syscall1:
	/*
	ldr	r1, [r0, #R1] //syscall id
	ldr	r0, [r0, #R0] //first param //ldm instead
	*/
	push	{lr}
	ldm.w	r0, {r0, r1}
	ldr	r2, =sys_vect
	ldr.w	r2, [r2, r1, lsl #2]
	blx	r2
	b	1f

	.balign 2
do_syscall2:
	push	{lr}
	ldm.w	r0, {r0-r2}
	ldr	r3, =sys_vect
	ldr.w	r3, [r3, r2, lsl #2]
	blx	r3
	b	1f

	.balign 2
do_syscall3:
	/*
	ldr	r3, [r0, #R3] //syscall id
	ldr	r2, [r0, #R2] //third
	ldr	r1, [r0, #R1] //second
	ldr	r0, [r0, #R0] //first param //ldm instead
	*/
	push	{lr}
	ldm.w	r0, {r0-r3}
	ldr	r12, =sys_vect
	ldr.w	r12, [r12, r3, lsl #2]
	blx	r12
	b	1f

1:	mrs	r1, psp
	str	r0, [r1, #R0]
	pop	{pc}
	
	.balign 2
do_syscall4:
	push	{r7, lr}
	ldm.w	r0, {r0-r3, r12}
	ldr	r7, =sys_vect
	ldr.w	r7, [r7, r12, lsl #2]
	blx	r7
	mrs	r1, psp
	str	r0, [r1, #R0]
	pop	{r7, pc}
ENDPROC(svcall)

	@ int dummy_sc_no_args(void)
ENTRY(sc_null)
	sc_no_params 0
	bx	lr
ENDPROC(sc_null)

	@ int pthread_yield(void)
ENTRY(pthread_yield)
	sc_no_params 1
	bx	lr
ENDPROC(pthread_yield)
