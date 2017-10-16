/*
 * kernel/exit.c
 *
 * Copyright (c) 2017 Baruch Marcot
 */

#include <kernel/list.h>
#include <kernel/mm/page.h>
#include <kernel/sched.h>
#include <kernel/syscalls.h>

#include <asm/current.h>

SYSCALL_DEFINE(exit, int status)
{
	/* current task becomes a zombie */
	current->state = EXIT_ZOMBIE;
	current->exit_code = status;

	release_task(current);

	schedule();

	/*
	 * exit() does not return. Task's resources are kept, just in case the
	 * parent thread needs to read child's stack, or the tasks's exit value.
	 * Releasing child task's resources can be done by any other task.
	 */

	//XXX: We should send a kernel signal to task's parent, and the
	// parent would release system resources (when? on its next scheduling?
	// Not great if the parent is blocked forever...)

	return 0; /* never reached */
}

extern struct list_head tasks;

SYSCALL_DEFINE(exit_group, int status)
{
	struct task_struct *task, *t;

	/* exit all threads in the calling process's thread group */
	//FIXME: Add a field to task_struct: struct list_head thread_group;
	list_for_each_entry_safe(task, t, &tasks, list) {
		if ((task != current) && (task->tgid == current->tgid)) {
			if (task->state == TASK_RUNNING)
				sched_dequeue(task);
			list_del(&task->list);
			free_pages((unsigned long)task->stack,
				size_to_page_order(THREAD_SIZE));
			release_task(task);
		}
	};

	/* current task becomes a zombie */
	current->state = EXIT_ZOMBIE;
	current->exit_code = status;
	release_task(current);

	schedule();

	return 0; /* never reached */
}
