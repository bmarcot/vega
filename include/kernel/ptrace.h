/*
 * include/kernel/ptrace.h
 *
 * Copyright (c) 2018 Benoit Marcot
 */

#ifndef _KERNEL_PTRACE_H
#define _KERNEL_PTRACE_H

#include <asm/ptrace.h>

struct pt_regs;
struct task_struct;

void dump_pt_regs(struct pt_regs *regs);
void dump_arch_context(struct task_struct *tsk);
void dump_task_context(struct task_struct *tsk);

#endif /* !_KERNEL_PTRACE_H */
