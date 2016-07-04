#include "kernel.h"
#include "linux/types.h"
#include "thread.h"
#include "utils.h"

extern void fault_enter(const char *s);
extern void fault_exit(void);

#define UFSR_DIVBYZERO (1 << 9)
#define UFSR_UNALIGNED (1 << 8)
#define UFSR_NOCP (1 << 3)
#define UFSR_INVPC (1 << 2)
#define UFSR_INVSTATE (1 << 1)
#define UFSR_UNDEFINSTR 1

void print_gprs(struct kernel_context_regs *noscratch,
		struct thread_context_regs *scratch, u32 exc_return)
{
	printk(" r0: %08x    r1: %08x    r2: %08x    r3: %08x\n", scratch->gprs[0],
		scratch->gprs[1], scratch->gprs[2], scratch->gprs[3]);
	printk(" r4: %08x    r5: %08x    r6: %08x    r7: %08x\n", noscratch->r4_r11[0],
		noscratch->r4_r11[1], noscratch->r4_r11[2], noscratch->r4_r11[3]);
	printk(" r8: %08x    r9: %08x   r10: %08x   r11: %08x\n", noscratch->r4_r11[4],
		noscratch->r4_r11[5], noscratch->r4_r11[6], noscratch->r4_r11[7]);
	printk("r12: %08x    sp: %08x    lr: %08x    pc: %08x\n", scratch->gprs[4],
		(u32) scratch, scratch->lr, scratch->ret_addr);
	printk("\nEXC_RETURN: %08x\n", exc_return);
}

void usagefault(struct kernel_context_regs *noscratch,
		struct thread_context_regs *scratch, u32 exc_return)
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
		printk("      ufsr: %08x  <%s>\n", ufsr, cause);
	else
		printk("      ufsr: %08x\n", ufsr);
	fault_exit();
}

void busfault(struct kernel_context_regs *noscratch,
	struct thread_context_regs *scratch, u32 exc_return)
{
	fault_enter("BusFault");
	print_gprs(noscratch, scratch, exc_return);
	fault_exit();
}

void memmanage(struct kernel_context_regs *noscratch,
	struct thread_context_regs *scratch, u32 exc_return)
{
	fault_enter("MemManage");
	print_gprs(noscratch, scratch, exc_return);
	fault_exit();
}
