/* syscall wrappers */

#include <string.h>

#include <uapi/kernel/signal.h>

#include <asm/syscalls.h>
#include "vega/syscalls.h"

#include "syscall-wrappers.h"

void sigreturn(void)
{
	SYS_sigreturn();
}

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
	return SYS_kill(pid, sig);
}

int sigqueue(pid_t pid, int sig, const union sigval value)
{
	return do_syscall3((void *)pid, (void *)sig, value.sival_ptr,
			SYS_SIGQUEUE);
}
