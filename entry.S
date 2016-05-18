
	.syntax unified
	.thumb

#include "linux/linkage.h"

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

	@ struct thread_info *current_thread_info(void)
ENTRY(current_thread_info)
	mov	r0, sp
	bfc	r0, #0, #INTR_STACK_SIZE_ORDER
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
END(thread_restore)
END(switch_to)

	 @ emulate a return from interrupt
ENTRY(return_from_irqaction)
	ldrd	r0, r1, [sp, #24]	/* r0 = ret_addr, r1 = xpsr */
	orr	r0, #1
	str	r0, [sp, #24]
	msr	apsr_nzcvq, r1		/* restore flags */
	pop	{r0-r3, r12, lr}
	ldr	pc, [sp], #8		/* return into user thread */
END(return_from_irqaction)
