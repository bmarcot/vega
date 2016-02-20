
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

	/* syscall call site */

	@ In order to not clobbering the parameter passing to function of the
	@ EABI, the SysCall ID is pushed as the last parameter in the param
	@ list of the called function. See the EABI specs to know which
	@ registers (and possibly the stack) are used during a function call.

        @ sys{cond}     argc, sys_id
#define DEFINE_SYS_CALL_SITE(cc)                \
        .macro sys##cc argc, id         ;       \
        .ifnb   cc                      ;       \
        itt     cc                      ;       \
        .endif                          ;       \
        .if \argc == 0                  ;       \
        mov##cc r0, #\id                ;       \
        .elseif \argc == 1              ;       \
        mov##cc r1, #\id                ;       \
        .elseif \argc == 2              ;       \
        mov##cc r2, #\id                ;       \
        .elseif \argc == 3              ;       \
        mov##cc r3, #\id                ;       \
        .elseif \argc == 4              ;       \
        mov##cc r12, #\id               ;       \
        .else                           ;       \
        .error "error: Unhandled number of parameters in SysCall." ;    \
        .endif                          ;       \
        svc##cc #\argc                  ;       \
        .endm

DEFINE_SYS_CALL_SITE()
DEFINE_SYS_CALL_SITE(eq)
DEFINE_SYS_CALL_SITE(ne)
DEFINE_SYS_CALL_SITE(cs)
DEFINE_SYS_CALL_SITE(cc)
DEFINE_SYS_CALL_SITE(mi)
DEFINE_SYS_CALL_SITE(pl)
DEFINE_SYS_CALL_SITE(vs)
DEFINE_SYS_CALL_SITE(vc)
DEFINE_SYS_CALL_SITE(hi)
DEFINE_SYS_CALL_SITE(ls)
DEFINE_SYS_CALL_SITE(ge)
DEFINE_SYS_CALL_SITE(lt)
DEFINE_SYS_CALL_SITE(gt)
DEFINE_SYS_CALL_SITE(le)

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

	/* dummy syscalls for testing-purpose */

ENTRY(dummy_0_args)
	sys	0, SYS_PTHREAD_CREATE + 1
	bx	lr
ENDPROC(dummy_0_args)

ENTRY(dummy_1_arg)
	sys	1, SYS_PTHREAD_CREATE + 2
	bx	lr
ENDPROC(dummy_1_arg)

ENTRY(dummy_2_args)
	sys	2, SYS_PTHREAD_CREATE + 3
	bx	lr
ENDPROC(dummy_2_args)

ENTRY(dummy_3_args)
	sys	3, SYS_PTHREAD_CREATE + 4
	bx	lr
ENDPROC(dummy_3_args)

ENTRY(dummy_4_args)
	sys	4, SYS_PTHREAD_CREATE + 5
	bx	lr
ENDPROC(dummy_4_args)
