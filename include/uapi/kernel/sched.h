/*
 * include/uapi/kernel/sched.h
 *
 * Copyright (c) 2017-2018 Benoit Marcot
 */

#ifndef _UAPI_KERNEL_SCHED_H
#define _UAPI_KERNEL_SCHED_H

#define CLONE_VM	0x00000100	/* set if VM shared between processes */
#define CLONE_FS	0x00000200	/* set if fs info shared between processes */
#define CLONE_FILES	0x00000400	/* set if open files shared between processes */
#define CLONE_SIGHAND	0x00000800	/* set if signal handlers and blocked signals shared */
#define CLONE_PTRACE	0x00002000	/* set if we want to let tracing continue on the child too */
#define CLONE_VFORK	0x00004000	/* set if the parent wants the child to wake it up on mm_release */
#define CLONE_PARENT	0x00008000	/* set if we want to have the same parent as the cloner */
#define CLONE_THREAD	0x00010000	/* Same thread group? */

#endif /* !_UAPI_KERNEL_SCHED_H */
