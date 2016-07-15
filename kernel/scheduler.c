#include <kernel/scheduler.h>
#include <kernel/thread.h>

extern const struct sched sched_rr;
extern const struct sched sched_o1;

static const struct sched *sched;

int sched_select(int sched_type)
{
	switch (sched_type) {
	case SCHED_CLASS_RR:
		sched = &sched_rr;
		break;
	case SCHED_CLASS_O1:
		sched = &sched_o1;
		break;
	default:
		return -1;
	}

	return sched->init();
}

int sched_add(struct thread_info *thread)
{
	return sched->add(thread);
}

int sched_del(struct thread_info *thread)
{
	return sched->del(thread);
}

int sched_elect(int flags)
{
	return sched->elect(flags);
}
