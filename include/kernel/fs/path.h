/*
 * include/kernel/fs/path.h
 *
 * Copyright (c) 2016-2018 Benoit Marcot
 */

#ifndef _KERNEL_FS_PATH_H
#define _KERNEL_FS_PATH_H

#include <kernel/types.h>

struct pathcomp {
	const char		*name;
	struct list_head	list;
};

int path_head(char *buf, const char *pathname);

/* Split happens in-place, memory pointed by pathname *IS* modified */
int path_split(struct list_head *head, char *pathname);

#endif /* !_KERNEL_FS_PATH_H */
