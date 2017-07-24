/*
 * include/uapi/kernel/dirent.h
 *
 * Copyright (c) 2017 Baruch Marcot
 *
 */

#ifndef _UAPI_KERNEL_DIRENT_H
#define _UAPI_KERNEL_DIRENT_H

#include <asm/posix_types.h>

struct dirent {
	ino_t d_ino;
	char  d_name[32];
};

#endif /* !_UAPI_KERNEL_DIRENT_H */
