#include <asm/syscalls.h>
#include "vega/syscalls.h"

#include <sys/types.h>

int getpriority(int which, id_t who)
{
	return do_syscall2((void *)which, (void *)who, SYS_GETPRIORITY);
}

int setpriority(int which, id_t who, int prio)
{
	return do_syscall3((void *)which, (void *)who, (void *)prio,
			SYS_SETPRIORITY);
}
