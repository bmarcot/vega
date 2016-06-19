#include "kernel.h"
#include "thread.h"
#include "utils.h"

extern void print_gprs(struct kernel_context_regs *noscratch,
		struct thread_context_regs *scratch, u32 exc_return);

void fault_enter(const char *s)
{
	printk("\n-------------------------------------------------------------\n");
	printk(" #%s\n\n", s);
}

void fault_exit(void)
{
	printk("-------------------------------------------------------------\n");
	halt();
}

void hardfault(struct kernel_context_regs *noscratch,
	struct thread_context_regs *scratch, u32 exc_return)
{
	fault_enter("HardFault");
	print_gprs(noscratch, scratch, exc_return);
	fault_exit();
}
