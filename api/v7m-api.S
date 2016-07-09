
	.syntax unified
	.thumb

#include "linux/linkage.h"

	.set SYS_PTHREAD_MUTEX_LOCK, 4
	.set SYS_PTHREAD_MUTEX_UNLOCK, 5

	// -1: unlocked, 0: locked, positive: locked, possible waiters

	@ int pthread_mutex_lock(pthread_mutex_t *mutex)
ENTRY(pthread_mutex_lock)
	movs	r2, #0
0:	ldrex	r1, [r0]
	tst	r1, r1
	itt	pl
	svcpl	#SYS_PTHREAD_MUTEX_LOCK
	bxpl	lr
	strex	r1, r2, [r0]
	tst	r1, r1
	bne	0b
	dmb			@ ARMv7-M ARM, A3.4.6
	movs	r0, #0
	bx	lr
ENDPROC(pthread_mutex_lock)

	@ int pthread_mutex_trylock(pthread_mutex_t *mutex)
ENTRY(pthread_mutex_trylock)
	movs	r2, #0
0:	ldrex	r1, [r0]
	tst	r1, r1
	itt	pl
	movspl	r0, #-1
	bxpl	lr
	strex	r1, r2, [r0]
	tst	r1, r1
	itt	ne
	movsne	r0, #-1
	bxne	lr
	dmb			@ ARMv7-M ARM, A3.4.6
	movs	r0, #0
	bx	lr
ENDPROC(pthread_mutex_trylock)

	@ int pthread_mutex_unlock(pthread_mutex_t *mutex)
ENTRY(pthread_mutex_unlock)
	movs	r2, #-1
0:	ldrex	r1, [r0]
	tst	r1, r1
	itt	ne
	svcne	#SYS_PTHREAD_MUTEX_UNLOCK
	bxne	lr
	strex	r1, r2, [r0]
	tst	r1, r1
	bne	0b
	dmb			@ ARMv7-M ARM, A3.4.6
	movs	r0, #0
	bx	lr
ENDPROC(pthread_mutex_lock)
