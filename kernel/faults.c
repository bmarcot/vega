#include <kernel/faults.h>
#include <kernel/thread.h>

#include "kernel.h"
#include "utils.h"
#include "platform.h"

void fault_enter(const char *s)
{
	printk("\n-------------------------------------------------------------\n");
	printk(" #%s\n\n", s);
}

void fault_exit(void)
{
	printk("-------------------------------------------------------------\n");
	__platform_halt();
}

void hardfault(struct v7m_kernel_ctx_regs *noscratch,
	struct v7m_thread_ctx_regs *scratch, u32 exc_return)
{
	fault_enter("HardFault");
	dump_frame(noscratch, scratch, exc_return);
	fault_exit();
}
