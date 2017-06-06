#include <kernel/syscalls.h>
#include "vega/syscalls.h"

int clone(int (*fn)(void *), void *child_stack, int flags, void *arg)
{
	return do_syscall4((void *)fn, (void *)child_stack, (void *)flags,
			(void *)arg, SYS_CLONE);
}
