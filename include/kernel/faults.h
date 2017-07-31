#ifndef KERNEL_FAULTS_H
#define KERNEL_FAULTS_H

#include <kernel/thread.h>
#include <asm/thread_info.h>

void fault_enter(const char *s);
void fault_exit(void);

/* arch-dependent (v6m, v7m) functions */
void dump_frame(struct cpu_user_context *ctx, u32 exc_return);

#endif /* !KERNEL_FAULTS_H */
