
	.syntax unified
	.thumb

#include "linux/linkage.h"

#include "../asm-pseudo.i"

#define SYS_PTHREAD_YIELD   0
#define SYS_PTHREAD_SELF    1
#define SYS_PTHREAD_EXIT    2
#define SYS_PTHREAD_CREATE  3

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
