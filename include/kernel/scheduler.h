#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <kernel/thread.h>

/* 0 <= PRI_MAX <= PRI_MIN */
#define PRI_MAX  0
#define PRI_MIN  31

#define SCHED_CLASS_RR  0
#define SCHED_CLASS_O1  1

#define SCHED_OPT_NONE          0
#define SCHED_OPT_RESTORE_ONLY  1
#define SCHED_OPT_RESET         2

struct sched {
	int (*init)(void);
	int (*enqueue)(struct thread_info *thread);
	int (*dequeue)(struct thread_info *thread);
	int (*elect)(int switch_type);
};

int sched_select(int sched_type);
int sched_enqueue(struct thread_info *thread);
int sched_dequeue(struct thread_info *thread);
int sched_elect(int flags);

#endif /* !SCHEDULER_H */
