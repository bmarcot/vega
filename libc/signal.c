/* syscall wrappers */

#include <string.h>
#include <signal.h>

#include <kernel/syscalls.h>
#include "vega/syscalls.h"

void sigreturn(void);

int sigaction(int sig, const struct sigaction *restrict act,
	struct sigaction *restrict oact)
{
	struct sigaction sa;

	memcpy(&sa, act, sizeof(struct sigaction));
	sa.sa_flags |= SA_RESTORER;
	sa.sa_restorer = sigreturn;

	return do_syscall3((void *)sig, &sa, (void *)oact, SYS_SIGACTION);
}

int _kill(pid_t pid, int sig)
{
	return do_syscall2((void *)pid, (void *)sig, SYS_KILL);
}

int sigqueue(pid_t pid, int sig, const union sigval value)
{
	return do_syscall3((void *)pid, (void *)sig, value.sival_ptr,
			SYS_SIGQUEUE);
}
