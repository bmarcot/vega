#ifndef KERNEL_FAULTS_H
#define KERNEL_FAULTS_H

#include <sys/types.h>
#include <kernel/thread.h>

void fault_enter(const char *s);
void fault_exit(void);

/* arch-dependent (v6m, v7m) functions */
void dump_frame(struct v7m_kernel_ctx_regs *noscratch,
		struct v7m_thread_ctx_regs *scratch, u32 exc_return);

#endif /* !KERNEL_FAULTS_H */
