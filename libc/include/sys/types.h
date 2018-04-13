#ifndef _SYS_TYPES_H
#define _SYS_TYPES_H

#include <asm/posix_types.h>
#include <uapi/kernel/time.h>

/*
 * Structure returned by gettimeofday(2) system call,
 * and used in other calls.
 */
struct timeval {
	long tv_sec; /* seconds */
	long tv_usec; /* and microseconds */
};

#endif /* !_SYS_TYPES_H */
