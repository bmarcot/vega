/* syscall wrappers */

#include <signal.h>

#include <kernel/syscalls.h>
#include "vega/syscalls.h"

int sigaction(int sig, const struct sigaction *restrict act,
	struct sigaction *restrict oact)
{
	return do_syscall3((void *)sig, (void *)act, (void *)oact, SYS_SIGACTION);
}

int raise(int sig)
{
	return do_syscall1((void *)sig, SYS_RAISE);
}

int sigqueue(pid_t pid, int sig, const union sigval value)
{
	return do_syscall3((void *)pid, (void *)sig, value.sival_ptr, SYS_SIGQUEUE);
}
