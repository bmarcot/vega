#include <stdio.h>
#include "linux/types.h"
#include "thread.h"
#include "utils.h"

#define UFSR_DIVBYZERO (1 << 9)
#define UFSR_UNALIGNED (1 << 8)
#define UFSR_NOCP (1 << 3)
#define UFSR_INVPC (1 << 2)
#define UFSR_INVSTATE (1 << 1)
#define UFSR_UNDEFINSTR 1

static void print_gprs(struct __intr_stackframe *noscratch, struct __thrd_stackframe *scratch,
		u32 exc_return)
{
	printf(" r0: %08x    r1: %08x    r2: %08x    r3: %08x\n", scratch->ts_gprs[0],
		scratch->ts_gprs[1], scratch->ts_gprs[2], scratch->ts_gprs[3]);
	printf(" r4: %08x    r5: %08x    r6: %08x    r7: %08x\n", noscratch->is_gprs[0],
		noscratch->is_gprs[1], noscratch->is_gprs[2], noscratch->is_gprs[3]);
	printf(" r8: %08x    r9: %08x   r10: %08x   r11: %08x\n", noscratch->is_gprs[4],
		noscratch->is_gprs[5], noscratch->is_gprs[6], noscratch->is_gprs[7]);
	printf("r12: %08x    sp: %08x    lr: %08x    pc: %08x\n", scratch->ts_gprs[5],
		(u32) scratch, scratch->ts_lr, scratch->ts_ret_addr);
	printf("\nEXC_RETURN: %08x\n", exc_return);
}

static void fault_enter(const char *s)
{
	printf("\n-------------------------------------------------------------\n");
	printf(" #%s\n\n", s);
}

static void fault_exit(void)
{
	printf("-------------------------------------------------------------\n");
	halt();
}

void hardfault(struct __intr_stackframe *noscratch, struct __thrd_stackframe *scratch,
	u32 exc_return)
{
	fault_enter("HardFault");
	print_gprs(noscratch, scratch, exc_return);
	fault_exit();
}

void usagefault(struct __intr_stackframe *noscratch, struct __thrd_stackframe *scratch,
		u32 exc_return)
{
	u32 ufsr = (*((volatile u32 *) 0xe000ed28)) >> 16;
	const char *cause = NULL;

	fault_enter("UsageFault");
	print_gprs(noscratch, scratch, exc_return);
	if (ufsr & UFSR_DIVBYZERO)
		cause = "DIVBYZERO";
	else if (ufsr & UFSR_UNALIGNED)
		cause = "UNALIGNED";
	else if (ufsr & UFSR_NOCP)
		cause = "NOCP";
	else if (ufsr & UFSR_INVPC)
		cause = "INVPC";
	else if (ufsr & UFSR_INVSTATE)
		cause = "INVSTATE";
	else if (ufsr & UFSR_UNDEFINSTR)
		cause = "UNDEFINSTR";
	if (cause)
		printf("      ufsr: %08x  <%s>\n", ufsr, cause);
	else
		printf("      ufsr: %08x\n", ufsr);
	fault_exit();
}

void busfault(struct __intr_stackframe *noscratch, struct __thrd_stackframe *scratch,
	u32 exc_return)
{
	fault_enter("BusFault");
	print_gprs(noscratch, scratch, exc_return);
	fault_exit();
}

void memmanage(struct __intr_stackframe *noscratch, struct __thrd_stackframe *scratch,
	u32 exc_return)
{
	fault_enter("MemManage");
	print_gprs(noscratch, scratch, exc_return);
	fault_exit();
}
