
	.syntax unified
	.thumb

#include "linux/linkage.h"

#include "../asm-pseudo.i"

	.set SYS_PTHREAD_YIELD, 0
	.set SYS_PTHREAD_SELF, 1
	.set SYS_PTHREAD_EXIT, 2
	.set SYS_PTHREAD_CREATE, 3
	.set SYS_PTHREAD_MUTEX_LOCK, 4
	.set SYS_PTHREAD_MUTEX_UNLOCK, 5
	.set SYS_TIMER_CREATE, 6

	@ int pthread_yield(void)
ENTRY(pthread_yield)
	sys	0, SYS_PTHREAD_YIELD
	bx	lr
ENDPROC(pthread_yield)

	@ pthread_t pthread_self(void)
ENTRY(pthread_self)
	sys	0, SYS_PTHREAD_SELF
	bx	lr
ENDPROC(pthread_self)

	@ void pthread_exit(void *retval)
ENTRY(pthread_exit)
	sys	1, SYS_PTHREAD_EXIT
	bx	lr
ENDPROC(pthread_exit)

	@ int pthread_create(...)
ENTRY(pthread_create)
	sys	3, SYS_PTHREAD_CREATE
	bx	lr
ENDPROC(pthread_create)

	// -1: unlocked, 0: locked, positive: locked, possible waiters

	@ int pthread_mutex_lock(pthread_mutex_t *mutex)
ENTRY(pthread_mutex_lock)
	movs	r2, #0
0:	ldrex	r1, [r0]
	tst	r1, r1
	ittt	pl		@ 't' 3x because 'sys' pseudo-op is 2 instructions
	syspl	1, SYS_PTHREAD_MUTEX_LOCK
	bxpl	lr
	strex	r1, r2, [r0]
	tst	r1, r1
	bne	0b
	dmb			@ ARMv7-M ARM, A3.4.6
	movs	r0, #0
	bx	lr
ENDPROC(pthread_mutex_lock)

	@ int pthread_mutex_unlock(pthread_mutex_t *mutex)
ENTRY(pthread_mutex_unlock)
	movs	r2, #-1
	dmb			@ ARMv7-M ARM, A3.4.6
0:	ldrex	r1, [r0]
	tst	r1, r1
	ittt	ne		@ 't' 3x because 'sys' pseudo-op is 2 instructions
	sysne	1, SYS_PTHREAD_MUTEX_UNLOCK
	bxne	lr
	strex	r1, r2, [r0]
	tst	r1, r1
	bne	0b
	dmb			@ ARMv7-M ARM, A3.4.6
	movs	r0, #0
	bx	lr
ENDPROC(pthread_mutex_lock)

	@ int timer_create(...)
ENTRY(timer_create)
	sys	1, SYS_TIMER_CREATE
	bx	lr
ENDPROC(timer_create)
