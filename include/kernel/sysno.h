/*
 * include/kernel/sysno.h
 *
 * Copyright (c) 2016 Benoit Marcot
 */

#ifndef KERNEL_SYSNO_H
#define KERNEL_SYSNO_H

#define SYS_PTHREAD_YIELD         0
#define SYS_PTHREAD_SELF          1
#define SYS_PTHREAD_EXIT          2
#define SYS_PTHREAD_CREATE        3
#define SYS_PTHREAD_MUTEX_LOCK    4
#define SYS_PTHREAD_MUTEX_UNLOCK  5
#define SYS_TIMER_CREATE          6
#define SYS_SYSCONF               7
#define SYS_PTHREAD_JOIN          8
#define SYS_PTHREAD_COND_SIGNAL   9
#define SYS_PTHREAD_COND_WAIT     10
#define SYS_MSLEEP                11
#define SYS_SIGACTION             12
#define SYS_RAISE                 13
#define SYS_SIGQUEUE              14

#endif /* !KERNEL_SYSNO_H */