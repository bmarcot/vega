/*
 * include/kernel/syscalls.h
 *
 * Copyright (c) 2016-2017 Benoit Marcot
 */

#ifndef _KERNEL_SYSCALLS_H
#define _KERNEL_SYSCALLS_H

#define SYSCALL_DEFINE(name, ...) \
	int sys_##name(__VA_ARGS__)

#endif /* !_KERNEL_SYSCALLS_H */
