
	.syntax unified
	.thumb

#include "linux/linkage.h"

	.set	INTR_STACK_SIZE_ORDER, 8

	@ struct thread_info *current_thread_info(void)
ENTRY(current_thread_info)
	mov	r0, sp
	ldr	r1, =0xffffff00
	ands	r0, r1
	bx	lr
ENDPROC(current_thread_info)

	@ do the thread-context switch
	@
	@ r0: struct thread_info *next
	@ r1: struct thread_info *prev (i.e. the current thread)
	@ (r1), r2, r3, r12: scratch registers
ENTRY(switch_to)
	/* save previous thread context */
	push	{r4-r7, lr}
	mov	r2, r8
	mov	r3, r9
	mov	r4, r10
	mov	r5, r11
	push	{r2-r5}
	mov	r2, sp
	mrs	r3, psp
	stm	r1!, {r2, r3}

ENTRY(thread_restore)
	/* restore next task context */
	ldm	r0!, {r1-r3}
	mov	sp, r1
	msr	psp, r2
	msr	control, r3		@ write to SPSEL is ignored in Handler_Mode
	pop	{r2-r5} //FIXME: r6 filled with r12? r12 is a scratch in the thread context, not but not in the interrupt/kernel context
	mov	r8, r2
	mov	r9, r3
	mov	r10, r4
	mov	r11, r5
	pop	{r4-r7, pc}
ENDPROC(thread_restore)
ENDPROC(switch_to)

	.set MC_SP, 0
	.set MC_LR, 4
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

	// save non scratch, restore all
	// need saving lr??

	.set MC_GPRS_R0, 8
	.set MC_GPRS_R4, 24
	.set MC_GPRS_R8, 40

ENTRY(swapcontext)
	/* save to oucp */
	adds	r0, #UC_MCONTEXT
	mov	r2, sp
	str	r2, [r0, #MC_SP]
	mov	r2, lr
	str	r2, [r0, #MC_LR]

	movs	r2, #MC_GPRS_R4
	adds	r2, r0            // save r4-r7
	stm	r2!, {r4-r7}
	mov	r2, r8
	mov	r3, r9
	mov	r4, r10
	mov	r5, r11
	mov	r6, r12
	mov	r7, lr
	adds	r0, #MC_GPRS_R8 // save r8-r12, lr
	stm	r0!, {r2-r7}

	/* retore from ucp */
	adds	r1, #UC_MCONTEXT
	ldr	r2, [r1, #MC_SP]
	mov	sp, r2
	ldr	r2, [r1, #MC_LR]
	mov	lr, r2

	// cannot ldm with pc in the list
	ldr	r2, [r1, #MC_PC]
	push	{r2}

	movs	r2, #MC_GPRS_R8
	adds	r2, r1           // from r8
	ldm	r2!, {r3-r7}
	mov	r3, r8
	mov	r4, r9
	mov	r5, r10
	mov	r6, r11
	mov	r7, r12
	adds	r1, #MC_GPRS_R0 // restore r0-r7
	ldm	r1, {r0-r7}
	pop	{pc}
ENDPROC(swapcontext)

ENTRY(return_from_makecontext)
	/* retrieve the struct& from the top of the stack */
	ldr	r0, [sp]

	/* get the machine struct for the linked context */
	ldr	r1, [r0, #UC_LINK]

	/* retore from ucp */
	adds	r1, #UC_MCONTEXT
	ldr	r2, [r1, #MC_SP]
	mov	sp, r2
	ldr	r2, [r1, #MC_LR]
	mov	lr, r2

	// cannot ldm with pc in the list
	ldr	r2, [r1, #MC_PC]
	push	{r2}

	movs	r2, #MC_GPRS_R8
	adds	r2, r1           // from r8
	ldm	r2!, {r3-r7}
	mov	r3, r8
	mov	r4, r9
	mov	r5, r10
	mov	r6, r11
	mov	r7, r12
	adds	r1, #MC_GPRS_R0 // restore r0-r7
	ldm	r1, {r0-r7}
	pop	{pc}
ENDPROC(return_from_makecontext)

	//FIXME: Should branch to HardFault high-level handler.
ENTRY(hardf)
0:	b	0b
ENDPROC(hardf)
