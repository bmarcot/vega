#include <stdlib.h>
#include <string.h>

#include <kernel/fs.h>
#include <kernel/kernel.h>

#include "linux/list.h"

extern struct file filetable[8];

extern struct inode rootdir_inodes[];

int sys_opendir(const char *name)
{
	return sys_open(name, O_DIRECTORY);
}

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
	struct file *file = &filetable[(int)dirp];
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

int sys_closedir(DIR *dirp)
{
	(void)dirp;

	return 0;
}

DIR *opendir(const char *name)
{
	return (DIR *)sys_opendir(name);
}

int readdir_r(DIR *dirp, struct dirent *entry, struct dirent **result)
{
	return sys_readdir_r(dirp, entry, result);
}

int closedir(DIR *dirp)
{
	return sys_closedir(dirp);
}
