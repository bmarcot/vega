#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "thread.h"

void sched_rr_add(struct thread_info *);
void sched_rr_del(struct thread_info *);
int sched_rr_elect(void);
int sched_rr_elect_reset(void);

#endif /* !SCHEDULER_H */
