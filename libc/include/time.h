#pragma once

#include <uapi/kernel/time.h>

#include "syscall-wrappers.h"

static inline int nanosleep(const struct timespec *req, struct timespec *rem)
{
	return SYS_nanosleep(req, rem);
}
