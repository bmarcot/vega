
	.syntax unified
	.thumb

#include "linux/linkage.h"

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

	/* syscall call site */

	@ In order to not clobbering the parameter passing to function of the
	@ EABI, the SysCall ID is pushed as the last parameter in the param
	@ list of the called function. See the EABI specs to know which
	@ registers (and possibly the stack) are used during a function call.

	.macro sys argc, id
	.if \argc == 0
	mov	r0, #\id
	.elseif \argc == 1
	mov	r1, #\id
	.elseif \argc == 2
	mov	r2, #\id
	.elseif \argc == 3
	mov	r3, #\id
	.elseif \argc == 4
	mov	r12, #\id
	.else
	.error "error: Unhandled number of parameters in SysCall."
	.endif
	svc	#\argc
	.endm

	/* syscall definitions */

#define SYS_PTHREAD_YIELD   0
#define SYS_PTHREAD_SELF    1
#define SYS_PTHREAD_EXIT    2
#define SYS_PTHREAD_CREATE  3

	@ int pthread_yield(void)
ENTRY(pthread_yield)
	sys	0, SYS_PTHREAD_YIELD
	bx	lr
ENDPROC(pthread_yield)

	@ pthread_t pthread_self(void)
ENTRY(pthread_self)
	sys	0, SYS_PTHREAD_SELF
	bx	lr
ENDPROC(pthread_self)

	@ void pthread_exit(void *retval)
ENTRY(pthread_exit)
	sys	1, SYS_PTHREAD_EXIT
	bx	lr
ENDPROC(pthread_exit)

	@ int pthread_create(...)
ENTRY(pthread_create)
	sys	3, SYS_PTHREAD_CREATE
	bx	lr
ENDPROC(pthread_create)
