#include "thread.h"
#include "irqaction.h"
#include "kernel.h"
#include "cmsis/arm/ARMCM4.h"

extern void return_from_irqaction(void);

void stage_irqaction(struct irqaction *irqaction, void *arg)
{
	CURRENT_THREAD_INFO(threadp);
	struct __thrd_stackframe *ts;

	/* If we stage the irqaction on the current thread context, note that
	 * the thread control block structure has not been updated with current
	 * SP_process.    */
	threadp->ti_psp = __get_PSP();

	/* the irqaction context will be poped by cpu on exception return */
	threadp->ti_psp -= sizeof (struct __thrd_stackframe);

	/* build the irqaction context */
	ts = (struct __thrd_stackframe *) threadp->ti_psp;
	ts->ts_gprs[0] = (u32) arg;
	ts->ts_gprs[1] = 0;
	ts->ts_gprs[2] = 0;
	ts->ts_gprs[3] = 0;
	ts->ts_gprs[4] = 0;
	ts->ts_lr = (u32) return_from_irqaction | 1;    /* return in Thumb Mode */
	ts->ts_ret_addr = (u32) irqaction->ia_irqaction & 0xfffffffe;
	ts->ts_xpsr = xPSR_T_Msk;

	/* If we staged the irqaction on the current thread context, update
	 * the SP_process before returning to thread.    */
	__set_PSP(threadp->ti_psp);
}
