/*
 * arch/arm/include/uapi/asm/posix_types.h
 *
 * Copyright (c) 2017-2018 Benoit Marcot
 */

#ifndef _ASM_POSIX_TYPES_H
#define _ASM_POSIX_TYPES_H

typedef int           clock_t;
typedef int           clockid_t;
typedef unsigned int	dev_t;	/* device number */
typedef unsigned int	ino_t;	/* inode number */
typedef int		off_t;	/* file offset */
typedef int           id_t;
typedef unsigned short	mode_t; /* permissions */
typedef int		pid_t;	/* process id */
typedef unsigned int  rlim_t;
typedef unsigned int  size_t;
typedef int           ssize_t;
typedef int           time_t;
typedef int           timer_t;

#endif /* !_ASM_POSIX_TYPES_H */
