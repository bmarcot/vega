/* syscall wrappers */

#include <string.h>

#include <uapi/kernel/signal.h>

#include <asm/syscalls.h>
#include <libvega/syscalls.h>

void sigreturn(void)
{
	syscall(0, SYS_SIGRETURN);
}

int sigaction(int sig, const struct sigaction *restrict act,
	struct sigaction *restrict oact)
{
	struct sigaction sa;

	memcpy(&sa, act, sizeof(struct sigaction));
	sa.sa_flags |= SA_RESTORER;
	sa.sa_restorer = sigreturn;

	return syscall(3, sig, &sa, oact, SYS_SIGACTION);
}

int _kill(pid_t pid, int sig)
{
	return syscall(2, pid, sig, SYS_KILL);
}

int sigqueue(pid_t pid, int sig, const union sigval value)
{
	return syscall(3, pid, sig, value.sival_ptr, SYS_SIGQUEUE);
}

int sigprocmask(int how, const sigset_t *set, sigset_t *oldset)
{
	return syscall(3, how, set, oldset, SYS_SIGPROCMASK);
}

int sigpending(sigset_t *set)
{
	return syscall(1, set, SYS_SIGPENDING);
}
