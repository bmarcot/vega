/*
 * kernel/resource.c
 *
 * Copyright (c) 2016-2017 Baruch Marcot
 */

#include <kernel/sched.h>
#include <kernel/types.h>

#include <asm/current.h>

#include <uapi/kernel/resource.h>

static struct rlimit rlimits[] = {
	{ .rlim_cur = 1024, .rlim_max = 1024 }  /* RLIMIT_STACK */
};

int sys_getrlimit(int resource, struct rlimit *rlim)
{
	rlim->rlim_cur = rlimits[resource].rlim_cur;
	rlim->rlim_max = rlimits[resource].rlim_max;

	return 0;
}

int sys_setrlimit(int resource, const struct rlimit *rlim)
{
	rlimits[resource].rlim_cur = rlim->rlim_cur;
	rlimits[resource].rlim_max = rlim->rlim_max;

	return 0;
}

#define PRIO_PROCESS 0

int sys_getpriority(int which, /* id_t */ int who)
{
	if (which != PRIO_PROCESS)
		return -1;

	if (who == current->pid) {
		return current->prio;
	} else {
		// find task by pid
	}

	return 0;
}

int sys_setpriority(int which, /* id_t */ int who, int prio)
{
	if (which != PRIO_PROCESS)
		return -1;

	if (who == current->pid) {
		current->prio = prio;
		return 0;
	} else {
		return -1; // find task by pid
		struct task_struct *tsk = 0;
		tsk->prio = prio;
		if (tsk->state == TASK_RUNNING) {
			sched_dequeue(tsk);
			sched_enqueue(tsk);
		}
	}

	return 0;
}
