#include <kernel/irqaction.h>
#include <kernel/thread.h>

#include "kernel.h"
#include "cmsis/arm/ARMCM4.h"

extern void return_from_irqaction(void);

void stage_irqaction(struct irqaction *irqaction, void *arg)
{
	CURRENT_THREAD_INFO(threadp);
	struct thread_context_regs *tcr;

	/* If we stage the irqaction on the current thread context, note that
	 * the thread control block structure has not been updated with current
	 * SP_process.    */
	threadp->ti_mach.mi_psp = __get_PSP();

	/* the irqaction context will be poped by cpu on exception return */
	threadp->ti_mach.mi_psp -= sizeof (struct thread_context_regs);

	/* build the irqaction context */
	tcr = (struct thread_context_regs *) threadp->ti_mach.mi_psp;
	tcr->r0_r3__r12[0] = (u32) arg;
	tcr->r0_r3__r12[1] = 0;
	tcr->r0_r3__r12[2] = 0;
	tcr->r0_r3__r12[3] = 0;
	tcr->r0_r3__r12[4] = 0;
	tcr->lr = (u32) return_from_irqaction | 1;    /* return in Thumb Mode */
	tcr->ret_addr = (u32) irqaction->ia_irqaction & 0xfffffffe;
	tcr->xpsr = xPSR_T_Msk;

	/* If we staged the irqaction on the current thread context, update
	 * the SP_process before returning to thread.    */
	__set_PSP(threadp->ti_mach.mi_psp);
}
