/* syscall wrappers */

#include <uapi/kernel/signal.h>
#include <kernel/types.h>
#include <uapi/kernel/time.h>

#include <asm/syscalls.h>
#include <libvega/syscalls.h>

int timer_create(clockid_t clockid, struct sigevent *sevp,
		timer_t *timerid)
{
	return syscall(3, clockid, sevp, timerid, SYS_TIMER_CREATE);
}

int timer_settime(timer_t timerid, int flags,
		const struct itimerspec *new_value,
		struct itimerspec *old_value)
{
	return syscall(4, timerid, flags, new_value, old_value, SYS_TIMER_SETTIME);
}

int timer_gettime(timer_t timerid, struct itimerspec *curr_value)
{
	return syscall(2, timerid, curr_value, SYS_TIMER_GETTIME);
}

int clock_gettime(clockid_t clk_id, struct timespec *tp)
{
	return syscall(2, clk_id, tp, SYS_CLOCK_GETTIME);
}
