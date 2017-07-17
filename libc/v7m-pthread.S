#include <kernel/syscalls.h>
#include <kernel/linkage.h>

#include <uapi/kernel/futex.h>

	.syntax unified
	.thumb

	//
	// Mutexes
	//

	// 1: unlocked, 0: locked, [n .. -1]: locked, possible waiters queued

	@ int pthread_mutex_lock(pthread_mutex_t *mutex)
ENTRY(pthread_mutex_lock)
0:	ldrex	r2, [r0]
	subs	r2, #1
	it	mi
	movmi	r2, #-1
	strex	r1, r2, [r0]
	tst	r1, r1
	bne	0b
	dmb			@ ARMv7-M ARM, A3.4.6
	tst	r2, r2
	itt	eq
	moveq	r0, #0
	bxeq	lr
	push	{r0}
	mov	r1, #FUTEX_WAIT
	mov	r3, #SYS_FUTEX
	svc	#3
	pop	{r0}
	b	0b
ENDPROC(pthread_mutex_lock)

	@ int pthread_mutex_trylock(pthread_mutex_t *mutex)
ENTRY(pthread_mutex_trylock)
0:	ldrex	r2, [r0]
	subs	r2, #1
	itt	mi
	movmi	r0, #-1
	bxmi	lr
	strex	r1, r2, [r0]
	tst	r1, r1
	bne	0b
	dmb			@ ARMv7-M ARM, A3.4.6
	mov	r0, #0
	bx	lr
ENDPROC(pthread_mutex_trylock)

	@ int pthread_mutex_unlock(pthread_mutex_t *mutex)
ENTRY(pthread_mutex_unlock)
0:	ldrex	r2, [r0]
	mov	r1, #1
	strex	r1, r1, [r0]
	tst	r1, r1
	bne	0b
	dmb			@ ARMv7-M ARM, A3.4.6
	tst	r2, r2
	itttt	mi
	movmi	r1, #FUTEX_WAKE
	movmi	r2, #0xff	// INT_MAX, THREAD_MAX
	movmi	r3, #SYS_FUTEX
	svcmi	#3
	mov	r0, #0
	bx	lr
ENDPROC(pthread_mutex_lock)

	//
	// Threads
	//

	@ int __pthread_trampoline(struct __pthread_struct *)
ENTRY(__pthread_trampoline)
	adr	lr, 0f
	orr	lr, #1		// set THUMB_BIT
	mov	sp, r0
	ldm.w	sp, {r0, pc}
0:	mov	r1, sp		// sp points to struct pthread
	b	__pthread_exit
ENDPROC(__pthread_trampoline)
