/*
 * arch/arm/kernel/ptrace.c
 *
 * Copyright (c) 2018 Benoit Marcot
 */

#include <kernel/sched.h>

#include <asm/ptrace.h>
#include <asm/thread_info.h>

void dump_pt_regs(struct pt_regs *regs)
{
	printk("  r0: %08x    r1: %08x    r2: %08x    r3: %08x\n",
		regs->r0, regs->r1, regs->r2, regs->r3);
	printk("  r4: %08x    r5: %08x    r6: %08x    r7: %08x\n",
		regs->r4, regs->r5, regs->r6, regs->r7);
	printk("  r8: %08x    r9: %08x   r10: %08x   r11: %08x\n",
		regs->r8, regs->r9, regs->r10, regs->r11);
	printk(" r12: %08x    sp: %08x    lr: %08x    pc: %08x\n",
		regs->r12, (u32)regs - sizeof(*regs), regs->lr, regs->pc);
	printk("xpsr: 0x%x <%s>\n", regs->status,
		regs->status & 0x1000000 ? "thumb_mode" : "arm");
}

void dump_arch_context(struct task_struct *tsk)
{
	struct thread_info *t = task_thread_info(tsk);

	dump_pt_regs(t->user.regs);

	u32 *stack = ((u32 *)(t->user.psp + sizeof(struct pt_regs)));
	printk("Stack: %08x %08x %08x %08x %08x %08x %08x %08x\n",
		*stack, *(stack + 1), *(stack + 2), *(stack + 3),
		*(stack+4), *(stack + 5), *(stack + 6), *(stack + 7));
	printk("       %08x %08x %08x %08x %08x %08x %08x %08x\n",
		*(stack+8), *(stack + 9), *(stack + 10), *(stack + 11),
		*(stack+12), *(stack + 13), *(stack + 14), *(stack + 15));
}
