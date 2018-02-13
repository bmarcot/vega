/*
 * include/uapi/kernel/wait.h
 *
 * Copyright (c) 2018 Benoit Marcot
 *
 */

#ifndef _UAPI_KERNEL_WAIT_H
#define _UAPI_KERNEL_WAIT_H

#define WNOHANG		0x00000001
#define WUNTRACED	0x00000002
#define WSTOPPED	WUNTRACED
#define WEXITED		0x00000004
#define WCONTINUED	0x00000008
#define WNOWAIT		0x01000000 /* Don't reap, just poll status.  */

#define __WNOTHREAD	0x20000000 /* Don't wait on children of other threads in this group */
#define __WALL		0x40000000 /* Wait on all children, regardless of type */
#define __WCLONE	0x80000000 /* Wait only on non-SIGCHLD children */

#endif /* _UAPI_KERNEL_WAIT_H */
