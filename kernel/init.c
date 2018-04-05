/*
 * kernel/init.c
 *
 * Copyright (c) 2018 Benoit Marcot
 */

#include <kernel/kernel.h>
#include <kernel/mm/page.h>
#include <kernel/sched.h>
#include <kernel/sched/signal.h>
#include <kernel/signal.h>
#include <kernel/signal_types.h>
#include <kernel/stddef.h>

#define CONFIG_INIT_STACK_SIZE	512 //FIXME: get stack size from??

int main(); //FIXME: unit tests support

static char stack[CONFIG_INIT_STACK_SIZE] __attribute__((aligned(8)));

static struct signal_struct signal;
static struct sighand_struct sighand;

struct task_struct *alloc_init_task(void)
{
	struct task_struct *init;

	init = alloc_pages(size_to_page_order(THREAD_SIZE));
	BUG_ON(!init);

	/* pids */
	reserve_pid(1);
	init->pid = 1;
	init->tgid = 1;

	init->prio = PRI_MIN;
	init->state = TASK_NEW;
	init->stack = &init->thread_info;
	init->flags = 0;

	init->group_leader = init;
	init->exit_signal = -1;
	init->parent = NULL;

	/* signals */
	for (int i = 0; i < _NSIG - 1; i++)
		sighand.action[i].sa_handler = SIG_DFL;
	INIT_LIST_HEAD(&signal.thread_head);
	init->signal = &signal;
	init->sighand = &sighand;
	//FIXME: Conflicting with unit tests, because they run as init process.
	// Unit tests should run as new process instead (started by exec()).
	sigemptyset(&init->blocked); //sigfillset(&init->blocked);
	init_sigpending(&init->pending);

	struct pt_regs regs = {	.pc = (u32)main, };
	arch_thread_setup(init, 0, stack + CONFIG_INIT_STACK_SIZE, &regs);

	add_task(init);
	sched_enqueue(init);

	return init;
}
