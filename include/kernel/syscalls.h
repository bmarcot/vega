/*
 * include/kernel/syscalls.h
 *
 * Copyright (c) 2016 Benoit Marcot
 */

#ifndef KERNEL_SYSCALLS_H
#define KERNEL_SYSCALLS_H

//XXX: GENERATED, DO NOT EDIT!

//FIXME: This belongs to include/uapi/

#define SYS_TIMER_CREATE 0
#define SYS_TIMER_SETTIME 1
#define SYS_TIMER_GETTIME 2
#define SYS_MSLEEP 3
#define SYS_SYSCONF 4
#define SYS_EXECVE 5
#define SYS_FORK 6
#define SYS_SIGACTION 7
#define SYS_KILL 8
#define SYS_SIGQUEUE 9
#define SYS_OPEN 10
#define SYS_CLOSE 11
#define SYS_READ 12
#define SYS_WRITE 13
#define SYS_LSEEK 14
#define SYS_STAT 15
#define SYS_MOUNT 16
#define SYS_READDIR_R 17
#define SYS_GETPID 18
#define SYS_MMAP 19
#define SYS_MUNMAP 20
#define SYS_CLONE 21
#define SYS_EXIT 22
#define SYS_FUTEX 23
#define SYS_SCHED_YIELD 24
#define SYS_GETPRIORITY 25
#define SYS_SETPRIORITY 26
#define SYS_SIGRETURN   27

#endif /* !KERNEL_SYSCALLS_H */
