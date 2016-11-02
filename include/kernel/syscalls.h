/*
 * include/kernel/syscalls.h
 *
 * Copyright (c) 2016 Benoit Marcot
 */

#ifndef KERNEL_SYSCALLS_H
#define KERNEL_SYSCALLS_H

//XXX: GENERATED TABLE, DO NOT EDIT FROM HERE!
//XXX: Change definitions in scripts/sysc.py
//XXX: Created on 2016-11-02 15:48
#define SYS_PTHREAD_EXIT 0
#define SYS_PTHREAD_SELF 1
#define SYS_PTHREAD_YIELD 2
#define SYS_PTHREAD_CREATE 3
#define SYS_PTHREAD_JOIN 4
#define SYS_PTHREAD_MUTEX_LOCK 5
#define SYS_PTHREAD_MUTEX_UNLOCK 6
#define SYS_PTHREAD_COND_SIGNAL 7
#define SYS_PTHREAD_COND_WAIT 8
#define SYS_TIMER_CREATE 9
#define SYS_MSLEEP 10
#define SYS_SYSCONF 11
#define SYS_SIGACTION 12
#define SYS_RAISE 13
#define SYS_SIGQUEUE 14
#define SYS_OPEN 15
#define SYS_READ 16
#define SYS_WRITE 17
#define SYS_SEEK 18
#define SYS_STAT 19
#define SYS_MOUNT 20

#endif /* !KERNEL_SYSCALLS_H */
