
	.syntax unified
	.thumb

#include <kernel/sysno.h>
#include "linux/linkage.h"

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

	@ int open()
ENTRY(open)
	svc	#SYS_OPEN
	bx	lr
ENDPROC(open)

	@ int read()
ENTRY(read)
	svc	#SYS_READ
	bx	lr
ENDPROC(read)

	@ int seek()
ENTRY(seek)
	svc	#SYS_SEEK
	bx	lr
ENDPROC(seek)

	@ int stat()
ENTRY(stat)
	svc	#SYS_STAT
	bx	lr
ENDPROC(stat)

	@ int mount()
ENTRY(mount)
	svc	#SYS_MOUNT
	bx	lr
ENDPROC(mount)
