/* syscall wrappers */

#include <signal.h>
#include <sys/types.h>

#include <kernel/syscalls.h>
#include "vega/syscalls.h"

int timer_create(clockid_t clockid, struct sigevent *sevp,
		timer_t *timerid)
{
	return do_syscall3((void *)clockid, (void *)sevp, (void *)timerid,
			SYS_TIMER_CREATE);
}

/* int timer_settime(timer_t timerid, int flags, */
/* 		const struct itimerspec *new_value, */
/* 		struct itimerspec * old_value) */
int timer_settime(timer_t timerid, int flags, int new_value)
{
	return do_syscall3((void *)timerid, (void *)flags, (void *)new_value,
			SYS_TIMER_SETTIME);

}
