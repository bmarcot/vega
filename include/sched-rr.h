#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "thread.h"

void sched_rr_add(struct thread_info *);
void sched_rr_elect(void);

#endif /* !SCHEDULER_H */
