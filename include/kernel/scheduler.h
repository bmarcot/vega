#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <kernel/thread.h>

#define SCHED_CLASS_RR  0
#define SCHED_CLASS_O1  1

#define SAVE_RESTORE  0
#define RESTORE_ONLY  1

struct sched {
	int (*init)(void);
	int (*add)(struct thread_info *thread);
	int (*del)(struct thread_info *thread);
	int (*elect)(int switch_type);
};

int sched_select(int sched_type);
int sched_add(struct thread_info *thread);
int sched_del(struct thread_info *thread);
int sched_elect(int switch_type);

#endif /* !SCHEDULER_H */
