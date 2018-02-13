#pragma once

#include <sys/types.h>

#include <uapi/kernel/wait.h>

pid_t waitpid(pid_t pid, int *status, int options);

/*
 * http://ab-initio.mit.edu/octave-Faddeeva/gnulib/lib/sys_wait.in.h
 *
 * WEXITSTATUS are bits 15..8 and WTERMSIG are bits 7..0
 */

#define WIFSIGNALED(x)	(WTERMSIG (x) != 0 && WTERMSIG(x) != 0x7f)
#define WIFEXITED(x)	(WTERMSIG (x) == 0)
#define WIFSTOPPED(x)	(WTERMSIG (x) == 0x7f)

/* The termination signal. Only to be accessed if WIFSIGNALED(x) is true.  */
#define WTERMSIG(x)	((x) & 0x7f)

/* The exit status. Only to be accessed if WIFEXITED(x) is true.  */
#define WEXITSTATUS(x)	(((x) >> 8) & 0xff)

/* The stopping signal. Only to be accessed if WIFSTOPPED(x) is true.  */
#define WSTOPSIG(x)	(((x) >> 8) & 0x7f)

/* True if the process dumped core.  Not standardized by POSIX.  */
#define WCOREDUMP(x)	((x) & 0x80)
