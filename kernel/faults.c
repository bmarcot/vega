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

void hardfault(struct cpu_user_context *ctx, u32 exc_return)
{
	fault_enter("HardFault");
	dump_frame(ctx, exc_return);
	fault_exit();
}
