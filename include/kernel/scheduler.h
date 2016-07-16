#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <kernel/thread.h>

#define SCHED_CLASS_RR  0
#define SCHED_CLASS_O1  1

#define SCHED_OPT_NONE          0
#define SCHED_OPT_RESTORE_ONLY  1
#define SCHED_OPT_RESET         2

struct sched {
	int (*init)(void);
	int (*add)(struct thread_info *thread);
	int (*del)(struct thread_info *thread);
	int (*elect)(int switch_type);
};

int sched_select(int sched_type);
int sched_add(struct thread_info *thread);
int sched_del(struct thread_info *thread);
int sched_elect(int flags);

#endif /* !SCHEDULER_H */
