#include <kernel/faults.h>
#include <kernel/thread.h>

#include "kernel.h"

void dump_frame(struct kernel_context_regs *noscratch,
		struct thread_context_regs *scratch, u32 exc_return)
{
	printk(" r0: %08x    r1: %08x    r2: %08x    r3: %08x\n", scratch->r0_r3__r12[0],
		scratch->r0_r3__r12[1], scratch->r0_r3__r12[2], scratch->r0_r3__r12[3]);
	printk(" r4: %08x    r5: %08x    r6: %08x    r7: %08x\n", noscratch->r4_r7[0],
		noscratch->r4_r7[1], noscratch->r4_r7[2], noscratch->r4_r7[3]);
	printk(" r8: %08x    r9: %08x   r10: %08x   r11: %08x\n", noscratch->r8_r11[0],
		noscratch->r8_r11[1], noscratch->r8_r11[2], noscratch->r8_r11[3]);
	printk("r12: %08x    sp: %08x    lr: %08x    pc: %08x\n", scratch->r0_r3__r12[4],
		(u32) scratch, scratch->lr, scratch->ret_addr);
	printk("\nEXC_RETURN: %08x\n", exc_return);
}
