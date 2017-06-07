/*
 * include/kernel/syscalls.h
 *
 * Copyright (c) 2016 Benoit Marcot
 */

#ifndef KERNEL_SYSCALLS_H
#define KERNEL_SYSCALLS_H

//XXX: GENERATED, DO NOT EDIT!

//FIXME: This belongs to include/uapi/

#define SYS_PTHREAD_EXIT 0
#define SYS_PTHREAD_SELF 1
#define SYS_PTHREAD_YIELD 2
#define SYS_PTHREAD_CREATE 3
#define SYS_PTHREAD_JOIN 4
#define SYS_PTHREAD_DETACH 5
#define SYS_PTHREAD_MUTEX_LOCK 6
#define SYS_PTHREAD_MUTEX_UNLOCK 7
#define SYS_PTHREAD_COND_SIGNAL 8
#define SYS_PTHREAD_COND_WAIT 9
#define SYS_TIMER_CREATE 10
#define SYS_TIMER_SETTIME 11
#define SYS_TIMER_GETTIME 12
#define SYS_MSLEEP 13
#define SYS_SYSCONF 14
#define SYS_EXECVE 15
#define SYS_FORK 16
#define SYS_SIGACTION 17
#define SYS_KILL 18
#define SYS_SIGQUEUE 19
#define SYS_OPEN 20
#define SYS_CLOSE 21
#define SYS_READ 22
#define SYS_WRITE 23
#define SYS_LSEEK 24
#define SYS_STAT 25
#define SYS_MOUNT 26
#define SYS_READDIR_R 27
#define SYS_GETPID 28
#define SYS_MMAP 29
#define SYS_MUNMAP 30
#define SYS_CLONE 31
#define SYS_EXIT 32

#endif /* !KERNEL_SYSCALLS_H */
