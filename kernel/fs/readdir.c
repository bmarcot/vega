/*
 * kernel/fs/readdir.c
 *
 * Copyright (c) 2016-2017 Benoit Marcot
 */

#include <stdlib.h>
#include <string.h>

#include <kernel/fs.h>
#include <kernel/kernel.h>

#include "linux/list.h"

static int fillonedir(struct dir_context *ctx, const char *name, int namlen,
		off_t offset, unsigned int ino, unsigned int d_type)
{
	(void)offset, (void)d_type;

	struct readdir_callback *buf =
		container_of(ctx, struct readdir_callback, ctx);
	struct vega_dirent *dirent = buf->dirent;

	dirent->d_ino = ino;
	strncpy(dirent->d_name, name, namlen);
	dirent->d_name[namlen] = '\0';

	return 0;
}

int sys_readdir_r(DIR *dirp, struct dirent *entry, struct dirent **result)
{
	struct file *file = fget((int)dirp);
	struct readdir_callback buf = {
		.ctx    = {.actor = fillonedir, .pos = 0},
		.dirent = (struct vega_dirent *)entry,
	};

	if (vfs_iterate(file, &buf.ctx))
		*result = NULL;
	else
		*result = entry;

	return 0;
}
