
	.syntax unified
	.thumb

#include "arch-v7m.h"
#include "linux/linkage.h"

	.include "macros.S"

	@ void	set_vtor(void *addr)
ENTRY(set_vtor)
	movl	r1, V7M_SCB_BASE
	str	r0, [r1, #V7M_VTOR]
	dsb	sy				//FIXME: in any order?
	isb	sy
	bx	lr
ENDPROC(set_vtor)

	/* Qemu does not implement the soft reset procedure.    */
	@ void	soft_reset(void)
	.balign	8
	.global soft_reset
soft_reset:
	movl	r1, V7M_SCB_BASE
	movl	r2, ((0x5fa << 16) | (1 << 2))
	dsb	sy
	str	r2, [r1, #V7M_AIRCR]
	dsb	sy
	b	cpu_locked
	.type	soft_reset, %function

	@ get task control block
	@ this works as long as the TCB is effectively at the
	@ base of the stack __and__ this is only one stack
	@ size, aligned on that size.
	//FIXME: how to handle different stack size?
	// store somewhere the current stack size info
	// but it is the kernel stack so we will never use a lot
	// of space !!!! :) user stack size must be configurable
	// though

	.set	INTR_STACK_SIZE_ORDER, 10

	.macro	_current_thread_info reg
	mov	\reg, sp
	bfc	\reg, #0, #INTR_STACK_SIZE_ORDER
	.endm

	@ struct thread_info *current_thread_info(void)
ENTRY(current_thread_info)
	_current_thread_info r0
	bx	lr
ENDPROC(current_thread_info)

	@ do the thread-context switch
	@
	@ r0: struct thread_info *next
	@ r1: struct thread_info *prev (i.e. the current thread)
	@ (r1), r2, r3, r12: scratch registers
ENTRY(switch_to)
	/* save previous thread context */
	push	{r4-r11, lr}
	mov	r2, sp
	mrs	r3, psp
	stm	r1!, {r2, r3}

ENTRY(thread_restore)
	/* restore next task context */
	ldm	r0!, {r1-r3}
	mov	sp, r1
	msr	psp, r2
	msr	control, r3		@ write to SPSEL is ignored in Handler_Mode
	pop	{r4-r11, pc}
ENDPROC(thread_restore)
ENDPROC(switch_to)
