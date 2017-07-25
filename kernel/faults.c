#include <kernel/faults.h>
#include <kernel/kernel.h>

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

struct preserved_context;
struct cpu_saved_context;

void hardfault(struct preserved_context *noscratch,
	struct cpu_saved_context *scratch, u32 exc_return)
{
	fault_enter("HardFault");
	dump_frame(noscratch, scratch, exc_return);
	fault_exit();
}
