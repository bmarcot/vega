/*
 * kernel/syscalls.c
 *
 * Copyright (c) 2016 Benoit Marcot
 */

#include <pthread.h>

#include <kernel/cond.h>
#include <kernel/mutex.h>
#include <kernel/syscalls.h>

int sys_pthread_yield(void);
pthread_t sys_pthread_self(void);
void sys_pthread_exit(void *);
int sys_pthread_create(/* __user */ pthread_t *thread, const pthread_attr_t *attr,
		void *(*start_routine)(void *), void *arg);
void sys_pthread_join(pthread_t, void **);
int sys_timer_create(unsigned int msecs);
long sys_sysconf(int name);

int sys_msleep();

/* signal declarations */

struct sigaction;
union sigval;

int sys_sigaction(int sig, const struct sigaction *restrict act,
	struct sigaction *restrict oact);
int sys_raise(int n);
int sys_sigqueue(pid_t pid, int sig, const union sigval value);

int sys_open();
int sys_read();
int sys_write();
int sys_seek();
int sys_stat();
int sys_mount();

#define SYS_MAX 48

void *syscall_vector[SYS_MAX] = {
	//XXX: GENERATED TABLE, DO NOT EDIT FROM HERE!
	//XXX: Change definitions in scripts/sysc.py
	//XXX: Created on 2016-11-02 15:48
	[SYS_PTHREAD_EXIT] = sys_pthread_exit,
	[SYS_PTHREAD_SELF] = sys_pthread_self,
	[SYS_PTHREAD_YIELD] = sys_pthread_yield,
	[SYS_PTHREAD_CREATE] = sys_pthread_create,
	[SYS_PTHREAD_JOIN] = sys_pthread_join,
	[SYS_PTHREAD_MUTEX_LOCK] = sys_pthread_mutex_lock,
	[SYS_PTHREAD_MUTEX_UNLOCK] = sys_pthread_mutex_unlock,
	[SYS_PTHREAD_COND_SIGNAL] = sys_pthread_cond_signal,
	[SYS_PTHREAD_COND_WAIT] = sys_pthread_cond_wait,
	[SYS_TIMER_CREATE] = sys_timer_create,
	[SYS_MSLEEP] = sys_msleep,
	[SYS_SYSCONF] = sys_sysconf,
	[SYS_SIGACTION] = sys_sigaction,
	[SYS_RAISE] = sys_raise,
	[SYS_SIGQUEUE] = sys_sigqueue,
	[SYS_OPEN] = sys_open,
	[SYS_READ] = sys_read,
	[SYS_WRITE] = sys_write,
	[SYS_SEEK] = sys_seek,
	[SYS_STAT] = sys_stat,
	[SYS_MOUNT] = sys_mount,
};

int syscall_register(unsigned ix, void *(*fn)())
{
	if (ix >= SYS_MAX) //SYSMAX = ARRAY_SIZE(syscall_vect)
		return -1;
	syscall_vector[ix] = fn;

	return 0;
}
