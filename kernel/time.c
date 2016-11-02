/* kernel/time.c
 *
 * Copyright (c) 2016 Benoit Marcot
 */

#include <if/time.h>

/* sleep() is usually part of <unistd.h> */

int sys_msleep(unsigned int msecs)
{
	return __msleep(msecs);
}
