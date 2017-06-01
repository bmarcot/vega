#include <kernel/sched.h>
#include <kernel/scheduler.h>
#include <kernel/thread.h>

extern const struct sched sched_o1;

static const struct sched *sched;

int sched_select(int sched_type)
{
	switch (sched_type) {
	case SCHED_CLASS_O1:
		sched = &sched_o1;
		break;
	default:
		return -1;
	}

	return sched->init();
}

int sched_enqueue(struct task_struct *task)
{
	task->ti_state = THREAD_STATE_READY;

	return sched->enqueue(&task->stack->thread_info);
}

int sched_dequeue(struct task_struct *task)
{
	return sched->dequeue(&task->stack->thread_info);
}

int sched_elect(int flags)
{
	int r;
	CURRENT_THREAD_INFO(cur_thread);

	KERNEL_STACK_CHECKING;

	r = sched->elect(flags);
	cur_thread->task->ti_state = THREAD_STATE_RUNNING;

	return r;
}
