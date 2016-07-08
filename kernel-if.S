
	//XXX: move this to uapi/

	.syntax unified
	.thumb

#include "linux/linkage.h"
#include "../pseudo.S"

	.set SYS_PTHREAD_YIELD, 0
	.set SYS_PTHREAD_SELF, 1
	.set SYS_PTHREAD_EXIT, 2
	.set SYS_PTHREAD_CREATE, 3
	.set SYS_PTHREAD_MUTEX_LOCK, 4
	.set SYS_PTHREAD_MUTEX_UNLOCK, 5
	.set SYS_TIMER_CREATE, 6
	.set SYS_SYSCONF, 7

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
