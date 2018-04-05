/*
 * kernel/idle.c
 *
 * Copyright (c) 2017 Baruch Marcot
 */

#include <kernel/kernel.h>
#include <kernel/sched.h>

static unsigned int	idle_stack[32];
struct task_struct	*idle_task;

void __do_idle(void);

static __attribute__((noreturn)) int do_idle(void *arg)
{
	for (;;)
		__do_idle();
}

int idle_init(void)
{
	/* idle_task is not added to the runqueue */
	struct pt_regs regs = { .pc = (u32)do_idle, };
	idle_task = do_clone(0, &idle_stack[32], &regs);
	if (!idle_task) {
		pr_err("Could not create the idle task");
		return -1;
	}
	pr_info("Created idle_thread at <%p> with pid=%d", idle_task, idle_task->tgid);

	return 0;
}
