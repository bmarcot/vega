
	.syntax unified
	.thumb

#include "linux/linkage.h"

	.set SYS_PTHREAD_YIELD, 0
	.set SYS_PTHREAD_SELF, 1
	.set SYS_PTHREAD_EXIT, 2
	.set SYS_PTHREAD_CREATE, 3
	.set SYS_PTHREAD_MUTEX_LOCK, 4
	.set SYS_PTHREAD_MUTEX_UNLOCK, 5
	.set SYS_TIMER_CREATE, 6
	.set SYS_SYSCONF, 7
	.set SYS_PTHREAD_JOIN, 8
	.set SYS_PTHREAD_COND_SIGNAL, 9
	.set SYS_PTHREAD_COND_WAIT, 10
	.set SYS_MSLEEP, 11
	.set SYS_SIGACTION, 12
	.set SYS_RAISE, 13
	.set SYS_SIGQUEUE, 14

	@ int pthread_yield(void)
ENTRY(pthread_yield)
	svc	#SYS_PTHREAD_YIELD
	bx	lr
ENDPROC(pthread_yield)

	@ pthread_t pthread_self(void)
ENTRY(pthread_self)
	svc	#SYS_PTHREAD_SELF
	bx	lr
ENDPROC(pthread_self)

	@ void pthread_exit(void *retval)
ENTRY(pthread_exit)
	svc	#SYS_PTHREAD_EXIT
	bx	lr
ENDPROC(pthread_exit)

	@ int pthread_create(...)
ENTRY(pthread_create)
	svc	#SYS_PTHREAD_CREATE
	bx	lr
ENDPROC(pthread_create)

	@ int timer_create(...)
ENTRY(timer_create)
	svc	#SYS_TIMER_CREATE
	bx	lr
ENDPROC(timer_create)

	@ long sysconf(int name)
ENTRY(sysconf)
	svc	#SYS_SYSCONF
	bx	lr
ENDPROC(sysconf)

	@ int pthread_join()
ENTRY(pthread_join)
	svc	#SYS_PTHREAD_JOIN
	bx	lr
ENDPROC(pthread_join)

	@ int pthread_cond_signal()
ENTRY(pthread_cond_signal)
	svc	#SYS_PTHREAD_COND_SIGNAL
	bx	lr
ENDPROC(pthread_cond_signal)

	@ int pthread_cond_wait()
ENTRY(pthread_cond_wait)
	svc	#SYS_PTHREAD_COND_WAIT
	bx	lr
ENDPROC(pthread_cond_wait)

	@ int msleep()
ENTRY(msleep)
	svc	#SYS_MSLEEP
	bx	lr
ENDPROC(msleep)

	@ int sigaction()
ENTRY(sigaction)
	svc	#SYS_SIGACTION
	bx	lr
ENDPROC(sigaction)

	@ int raise()
ENTRY(raise)
	svc	#SYS_RAISE
	bx	lr
ENDPROC(raise)

	@ int sigqueue()
ENTRY(sigqueue)
	svc	#SYS_SIGQUEUE
	bx	lr
ENDPROC(sigqueue)
