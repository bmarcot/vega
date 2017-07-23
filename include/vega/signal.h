#ifndef LIBC_SIGNAL_H
#define LIBC_SIGNAL_H

#include <sys/types.h>

#include <kernel/types.h>

#include <uapi/kernel/signal.h>

typedef struct {
	void *ss_sp;     /* Base address of stack */
	int ss_flags;    /* Flags */
	size_t ss_size;	 /* Number of bytes in stack */
} stack_t;

#define SIGKILL  9   /* Kill (can't be caught or ignored) (POSIX) */
#define SIGUSR1  10  /* User defined signal 1 (POSIX) */
#define SIGUSR2  12  /* User defined signal 2 (POSIX) */
#define SIGSTOP  19  /* Stop executing(can't be caught or ignored) (POSIX) */

int sigaction(int sig, const struct sigaction *restrict act,
	struct sigaction *restrict oact);
int raise(int sig);
int sigqueue(pid_t pid, int sig, const union sigval value);

#endif /* !LIBC_SIGNAL_H */
