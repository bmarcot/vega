/*
 * arch/arm/kernel/v7m-entry.S
 *
 * Copyright (c) 2016-2017 Benoit Marcot
 */

#include <kernel/linkage.h>
#include <asm/assembler.h>

	.syntax unified
	.thumb

	@ resume -- do the actual context-switch
	@
	@ r0: struct thread_info *next
	@ r1: struct thread_info *prev
	@ r2: int save_prev
ENTRY(resume)
	tst	r2, r2			/* save previous thread if not zero */
	ittt	ne
	pushne	{r4-r12, lr}
	movne	r2, sp
	strne	r2, [r1]

	ldr	sp, [r0]		/* restore next thread */
	ldr	r2, [r0, #8]
	msr	control, r2		/* write to SPSEL is ignored in Handler_Mode */
	mov	r0, r1
	pop	{r4-r12, pc}
ENDPROC(resume)

	.macro	emulate_iret
	ldrd	r0, r1, [sp, #24]	/* r0 = ret_addr, r1 = xpsr */
	orr	r0, #1
	str	r0, [sp, #24]
	msr	apsr_nzcvq, r1		/* restore flags */
	pop	{r0-r3, r12, lr}
	ldr	pc, [sp], #8		/* return into user thread */
	.endm

ENTRY(v7m_task_start_trampoline)
	mov	r0, sp
	bfc	r0, #0, #9
	ldr	r0, [r0, #4] 		//FIXME: offsetof(thread_ctx->sp)
	ldmia	r0!, {r4-r11}
	msr	psp, r0
	mov32	r0, 0xfffffffd
	bx	r0
ENDPROC(v7m_task_start_trampoline)

	.set MC_SP, 0
	.set MC_LR, 4
	.set MC_GPRS, 8
	.set MC_PC, 15*4
	.set UC_MCONTEXT, 16
	.set UC_LINK, 0

	/* Swap context to a clean context (created from makecontext()):
	 *   - save the non-scratch registers
	 *   - just restore scratch registers (for arguments to entry function)
	 *
	 * Swap to a dirty context (inited with getcontext, or return to a
	 * swapped context):
	 *   - save the non-scratch registers
	 *   - restore all registers
	 */

ENTRY(swapcontext)
	/* save to oucp */
	add	r0, #UC_MCONTEXT
	str	sp, [r0, #MC_SP]
	str	lr, [r0, #MC_LR]
	add	r0, #MC_GPRS
	stm	r0, {r0-r12, lr}

	/* retore from ucp */
	add	r1, #UC_MCONTEXT
	ldr	sp, [r1, #MC_SP]
	ldr	lr, [r1, #MC_LR]
	add	r1, #MC_GPRS
	ldm	r1, {r0-r12, pc}
ENDPROC(swapcontext)

ENTRY(return_from_makecontext)
	/* retrieve the struct& from the top of the stack */
	ldr	r0, [sp]

	/* get the machine struct for the linked context */
	ldr	r0, [r0, #UC_LINK]
	add	r0, #UC_MCONTEXT

	/* restore the link context */
	ldr	sp, [r0, #MC_SP]
	ldr	lr, [r0, #MC_LR]
	add	r0, #MC_GPRS
	//FIXME: return code - the r0 value that is restored from the stack should be 0
	ldm	r0, {r0-r12, pc}
ENDPROC(return_from_makecontext)

	.macro	fault_handler f
	push	{r4-r11}	@ save non-scratch registers at time of fault
	mov	r0, sp
	mov	r2, lr		@ lr contains EXC_RETURN, fault was taken in thread or interrupt?

	@ We don't handle faults taken in interrupt handler at the moment.
	@ Hence the scratch registers auto pushed to the stack by the CPU
	@ on interrupt-entry have been pushed to PSP (MSP is only used by
	@ interrupts handler).
	mrs	r1, psp

	b	\f
	.endm	/* fault_handler */

ENTRY(hardf)
	fault_handler hardfault
ENDPROC(hardf)

ENTRY(memf)
	fault_handler memmanage
ENDPROC(memf)

ENTRY(busf)
	fault_handler busfault
ENDPROC(busf)

ENTRY(usgf)
	fault_handler usagefault
ENDPROC(usgf)

ENTRY(__do_idle)
	wfi
	bx	lr
ENDPROC(__do_idle)
