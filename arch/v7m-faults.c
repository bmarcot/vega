#include <kernel/faults.h>
#include <kernel/thread.h>

#include <asm/thread_info.h>

#include "kernel.h"

#define UFSR_DIVBYZERO (1 << 9)
#define UFSR_UNALIGNED (1 << 8)
#define UFSR_NOCP (1 << 3)
#define UFSR_INVPC (1 << 2)
#define UFSR_INVSTATE (1 << 1)
#define UFSR_UNDEFINSTR 1

void dump_frame(struct preserved_context *noscratch,
		struct cpu_saved_context *scratch, u32 exc_return)
{
	printk(" r0: %08x    r1: %08x    r2: %08x    r3: %08x\n", scratch->r0_r3__r12[0],
		scratch->r0_r3__r12[1], scratch->r0_r3__r12[2], scratch->r0_r3__r12[3]);
	printk(" r4: %08x    r5: %08x    r6: %08x    r7: %08x\n",
		noscratch->r4, noscratch->r5, noscratch->r6, noscratch->r7);
	printk(" r8: %08x    r9: %08x   r10: %08x   r11: %08x\n",
		noscratch->r8, noscratch->r9, noscratch->r10, noscratch->r11);
	printk("r12: %08x    sp: %08x    lr: %08x    pc: %08x\n", scratch->r0_r3__r12[4],
		(u32) scratch, scratch->lr, scratch->ret_addr);
	printk("\nEXC_RETURN: %08x\n", exc_return);
}

void usagefault(struct preserved_context *noscratch,
		struct cpu_saved_context *scratch, u32 exc_return)
{
	u32 ufsr = (*((volatile u32 *) 0xe000ed28)) >> 16;
	const char *cause = NULL;

	fault_enter("UsageFault");
	dump_frame(noscratch, scratch, exc_return);
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
		printk("      ufsr: %08x  <%s>\n", ufsr, cause);
	else
		printk("      ufsr: %08x\n", ufsr);
	fault_exit();
}

void busfault(struct preserved_context *noscratch,
	struct cpu_saved_context *scratch, u32 exc_return)
{
	fault_enter("BusFault");
	dump_frame(noscratch, scratch, exc_return);
	fault_exit();
}

void memmanage(struct preserved_context *noscratch,
	struct cpu_saved_context *scratch, u32 exc_return)
{
	fault_enter("MemManage");
	dump_frame(noscratch, scratch, exc_return);
	fault_exit();
}
