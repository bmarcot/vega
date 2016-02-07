#include <stdio.h>

#include "linux/types.h"
#include "thread.h"

void hardfault(struct __intr_stackframe *noscratch, struct __thrd_stackframe *scratch, u32 exc_return)
{
	printf("\n-------------------------------------------------------------\n");
	printf("#hardfault\n\n");
	printf(" r0: %08x    r1: %08x    r2: %08x    r3: %08x\n", scratch->ts_gprs[0],
		scratch->ts_gprs[1], scratch->ts_gprs[2], scratch->ts_gprs[3]);
	printf(" r4: %08x    r5: %08x    r6: %08x    r7: %08x\n", noscratch->is_gprs[0],
		noscratch->is_gprs[1], noscratch->is_gprs[2], noscratch->is_gprs[3]);
	printf(" r8: %08x    r9: %08x   r10: %08x   r11: %08x\n", noscratch->is_gprs[4],
		noscratch->is_gprs[5], noscratch->is_gprs[6], noscratch->is_gprs[7]);
	printf("r12: %08x    sp: %08x    lr: %08x    pc: %08x\n", scratch->ts_gprs[5],
		(u32) scratch, scratch->ts_lr, scratch->ts_ret_addr);
	printf("\nEXC_RETURN: <%08x>\n", exc_return);
	printf("-------------------------------------------------------------\n");

	for (;;)
		;
}
