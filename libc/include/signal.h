#ifndef _LIBC_SIGNAL_H
#define _LIBC_SIGNAL_H

#include <sys/types.h>
#include <uapi/kernel/signal.h>

typedef struct {
	void   *ss_sp;
	int    ss_flags;
	size_t ss_size;
} stack_t;

int sigaction(int sig, const struct sigaction *restrict act,
	struct sigaction *restrict oact);
int raise(int sig);
int sigqueue(pid_t pid, int sig, const union sigval value);

#endif /* !_LIBC_SIGNAL_H */
