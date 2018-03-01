/*
 * include/uapi/kernel/sched.h
 *
 * Copyright (c) 2017-2018 Benoit Marcot
 */

#ifndef _UAPI_KERNEL_SCHED_H
#define _UAPI_KERNEL_SCHED_H

#define CLONE_VFORK	0x00004000	/* child waked up parent on exit */
#define CLONE_SIGHAND	0x00000800	/* set if signal handlers and blocked signals shared */
#define CLONE_THREAD	0x00010000	/* same thread group? */

#endif /* !_UAPI_KERNEL_SCHED_H */
