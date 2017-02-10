/*
 * kernel/fs/romfs.c
 *
 * Copyright (c) 2016-2017 Benoit Marcot
 */

/* https://www.kernel.org/doc/Documentation/filesystems/romfs.txt */

#include <inet.h> /* words are big endian in romfs */
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#include <kernel/fs.h>
#include <kernel/fs/romfs.h>
#include <kernel/kernel.h>

#include <drivers/mtd/mtd.h>

#include "linux/list.h"

const struct inode_operations romfs_iops;
const struct file_operations  romfs_fops;

static const char *basename(const char *name)
{
	const char *p = name;

	while (*p != '\0')
		p++;
	while (p > name && *(p - 1) != '/')
		p--;

	return p;
}

// mount("/dev/mtd", "/media/flash", "romfs", 0, NULL);
int romfs_mount(const char *source, const char *target,
		const char *filesystemtype,
		unsigned long mountflags, const void *data)
{
	(void)filesystemtype, (void)mountflags, (void)data;

	struct inode *s_inode = inode_from_pathname(source);
	if (s_inode == NULL)
		return -1;

	//FIXME: Use mkdir() or create()
	struct inode *inode = malloc(sizeof(struct inode));
	if (inode == NULL)
		return -1;
	init_tmpfs_inode(inode);
	inode->i_op = &romfs_iops;
	inode->i_mode = S_IFDIR;
	inode->i_size = 0;
	inode->i_private = s_inode->i_private; //FIXME: Use i_dev

	// link mounted-over inode to parent directory
	struct dentry dentry;
	printk("Creating /dev/%s\n", basename(target));
	dentry.d_inode = inode;
	strcpy(dentry.d_name, basename(target));
	vfs_link(0, dev_inode(), &dentry);

	return 0;
}

static struct romfs_inode *first_filehdr(struct romfs_superblock *super)
{
	/* volume_name is a 0-terminated string */
	int volname_len = align_next(strlen(super->volume_name) + 1, 16);

	return (struct romfs_inode *)(super->volume_name + volname_len);
}

static void *offset_in_dev(struct romfs_superblock *super,
			struct romfs_inode *rinode)
{
	/* file_name is a 0-terminated string */
	int filename_len = align_next(strlen(rinode->file_name) + 1, 16);

	return (void *)((char *)rinode->file_name - (char *)super
			+ filename_len);
}

struct dentry *romfs_lookup(struct inode *dir, struct dentry *target)
{
	static int ino = 0xbeef;
	struct mtd_info *mtd = dir->i_private;
	struct romfs_superblock *super = mtd->priv;
	struct romfs_inode *rinode = first_filehdr(super);

	for (int i = 0; i < /* MAX_FILES_PER_DEV */10; i++) {
		if (!strcmp(rinode->file_name, target->d_name)) {
			//FIXME: Flat filesystem
			struct inode *inode = malloc(sizeof(struct inode));
			if (inode == NULL)
				return NULL;
			inode->i_ino = ino++;
			inode->i_mode = S_IFREG;
			inode->i_size = ntohl(rinode->size);
			inode->i_fop = &romfs_fops;
			inode->i_private = offset_in_dev(super, rinode);

			target->d_inode = inode;
			target->d_count = 0;

			return target;
		}

		u32 next_filehdr = align(ntohl(rinode->next_filehdr), 16);
		if (!next_filehdr)
			break;
		rinode = (struct romfs_inode *)((char *)super + next_filehdr);
	}

	return NULL;
}

void dump_romfs_info(struct romfs_superblock *super)
{
	printk("Superblock:\n");
	printk("    Volume name  %s\n", super->volume_name);
	printk("    Full size    %d bytes\n", ntohl(super->full_size));

	struct romfs_inode *rinode = first_filehdr(super);
	for (int i = 0; i < /* MAX_FILES_PER_DEV */10; i++) {
		printk("Inode:\n");
		printk("    File name    %s\n", rinode->file_name);
		printk("    File size    %d bytes\n", ntohl(rinode->size));

		u32 next_filehdr = align(ntohl(rinode->next_filehdr), 16);
		if (!next_filehdr)
			break;
		rinode = (struct romfs_inode *)((char *)super + next_filehdr);
	}
}

int romfs_open(struct inode *inode, struct file *file)
{
	file->f_private = inode->i_private;

	return 0;
}

ssize_t romfs_read(struct file *file, char *buf, size_t count, off_t offset)
{
	size_t retlen;
	size_t filesize = file->f_dentry->d_inode->i_size;

	//FIXME: Use file->sb, file->dev
	struct mtd_info *mtd = file->f_dentry->d_parent->d_inode->i_private;

	if (file->f_pos + count > filesize)
		count = filesize - offset;
	mtd_read(mtd, (off_t)file->f_private + offset, count, &retlen,
		(unsigned char *)buf);

	return retlen;
}

const struct inode_operations romfs_iops = {
	.lookup = romfs_lookup,
};

const struct file_operations romfs_fops = {
	.open = romfs_open,
	.read = romfs_read,
};
