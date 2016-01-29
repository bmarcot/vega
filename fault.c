#include <stdio.h>

#include "linux/types.h"
#include "thread.h"

void hardfault(struct __intr_stackframe *noscratch, struct __thrd_stackframe *scratch, u32 exc_return)
{
	printf("\n#hardfault\n");
	printf("r0         = %08x\n", scratch->ts_gprs[0]);
	printf("r4         = %08x\n", noscratch->is_gprs[0]);
	printf("lr         = %08x\n", scratch->ts_lr);
	printf("pc         = %08x\n", scratch->ts_ret_addr);
	printf("EXC_RETURN = %08x\n", exc_return);

	for (;;)
		;
}
