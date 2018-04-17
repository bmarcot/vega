/*
 * include/kernel/elf.h
 *
 * Copyright (c) 2017-2018 Benoit Marcot
 */

#ifndef _KERNEL_ELF_H
#define _KERNEL_ELF_H

struct task_struct;

int elf_load_binary(const char *pathname, struct task_struct *tsk);
int elf_exec_binary(const char *pathname);

#endif /* !_KERNEL_ELF_H */
