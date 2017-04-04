/*
 * kernel/fs/romfs.c
 *
 * Copyright (c) 2016-2017 Benoit Marcot
 */

/* https://www.kernel.org/doc/Documentation/filesystems/romfs.txt */

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <vega/arpa/inet.h> /* words are big endian in romfs */

#include <kernel/fs.h>
#include <kernel/fs/romfs.h>
#include <kernel/kernel.h>

#include <drivers/mtd/mtd.h>

#include "linux/list.h"

const struct inode_operations romfs_iops;
const struct file_operations romfs_fops;
const struct dentry_operations romfs_dops;

static char *basename(const char *filename)
{
	char *p = strrchr(filename, '/');

	return p ? p + 1 : (char *)filename;
}

static off_t offsetof_device_inode(struct romfs_inode *rinode,
				struct romfs_superblock *super)
{
	return (off_t)rinode - (off_t)super;
}

static off_t offsetof_first_device_inode(struct romfs_superblock *super)
{
	/* volume_name is a null-terminated string */
	int len = align_next(strlen(super->volume_name) + 1, 16);

	return offsetof(struct romfs_superblock, volume_name) + len;
}

static struct romfs_inode *get_romfs_inode(struct inode *inode)
{
	void *romfs_inode;
	size_t retlen;
	int err;
	struct mtd_info *mtd;

	mtd = inode->i_sb->s_private;
	err = mtd_point(mtd, (off_t)inode->i_private,
			sizeof(struct romfs_inode), &retlen, &romfs_inode);
	if (err || (retlen != sizeof(struct romfs_inode)))
		return NULL;

	return romfs_inode;
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

	// link mounted-over inode to parent directory
	struct dentry dentry;
	printk("Creating /dev/%s\n", basename(target));
	dentry.d_inode = inode;
	strcpy(dentry.d_name, basename(target));
	vfs_link(0, dev_inode(), &dentry);

	/* Allocate a super_block struct that will be released on filesystem
	 * unmount. */
	struct super_block *super_block = malloc(sizeof(struct super_block));
	if (super_block == NULL)
		return -1;
	/* super_block is found at the begining of memory area on MTD dev */
	struct mtd_info *mtd = s_inode->i_private;
	super_block->s_private = mtd;
	super_block->s_iroot = inode; //FIXME: super_block must point to dentry instead of inode
	inode->i_sb = super_block;
	struct romfs_superblock *super = mtd->priv;
	inode->i_private = (void *)offsetof_first_device_inode(super);

	return 0;
}

struct dentry *romfs_lookup(struct inode *dir, struct dentry *target)
{
	static int ino = 0xbeef;
	u32 next_filehdr;
	struct mtd_info *mtd = dir->i_sb->s_private;
	struct romfs_superblock *super = mtd->priv;
	struct romfs_inode *rinode = get_romfs_inode(dir);

	/* enter and walk the directory */
	next_filehdr = align(ntohl(rinode->spec_info), 16);
	rinode = (struct romfs_inode *)((char *)super + next_filehdr);

	for (int i = 0; i < /* MAX_FILES_PER_DEV */10; i++) {
		if (!strcmp(rinode->file_name, target->d_name)) {
			/* populate a new inode */
			struct inode *inode = malloc(sizeof(struct inode));
			if (inode == NULL)
				return NULL;
			if ((ntohl(rinode->next_filehdr) & 0x7) == 1)
				inode->i_mode = S_IFDIR;
			else
				inode->i_mode = S_IFREG;
			inode->i_ino = ino++;
			inode->i_size = ntohl(rinode->size);
			inode->i_op = &romfs_iops;
			inode->i_fop = &romfs_fops;
			/* We store the offset to on-device inode rather than
			 * the logical address of the on-device inode, because
			 * that does not work if fs is stored on a SPI flash
			 * for instamce, and is not directly mapped onto logical
			 *  address space. */
			inode->i_private =
				(void *)offsetof_device_inode(rinode, super);
			inode->i_sb = dir->i_sb;

			/* update the dentry */
			target->d_inode = inode;
			target->d_op = &romfs_dops;

			return target;
		}

		/* inspect next file in current directory */
		next_filehdr = align(ntohl(rinode->next_filehdr), 16);
		if (!next_filehdr)
			break;
		rinode = (struct romfs_inode *)((char *)super + next_filehdr);
	}

	return NULL;
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
	struct mtd_info *mtd = file->f_dentry->d_inode->i_sb->s_private;
	struct romfs_inode *rinode =
		get_romfs_inode(file->f_dentry->d_inode);
	int len = sizeof(struct romfs_inode)
		+ align_next(strlen(rinode->file_name) + 1, 16);

	if (file->f_pos + count > filesize)
		count = filesize - offset;
	mtd_read(mtd, (off_t)file->f_dentry->d_inode->i_private + len + offset,
		count, &retlen, (unsigned char *)buf);

	return retlen;
}

int romfs_mmap(struct file *file, off_t offset, void **addr)
{
	size_t retlen;
	size_t filesize = file->f_dentry->d_inode->i_size;
	struct mtd_info *mtd = file->f_dentry->d_inode->i_sb->s_private;
	struct romfs_inode *rinode =
		get_romfs_inode(file->f_dentry->d_inode);
	int len = sizeof(struct romfs_inode)
		+ align_next(strlen(rinode->file_name) + 1, 16);

	return mtd_point(mtd,
			(off_t)file->f_dentry->d_inode->i_private + len + offset,
			filesize, &retlen, addr);
}

int romfs_delete(struct dentry *dentry)
{
	/* release in-memory inode */
	/* the root inode is deleted on unmount(), operation is pointed
	 * by i_sb->s_op->unmount() */
	if (dentry->d_inode != dentry->d_inode->i_sb->s_iroot)
		free(dentry->d_inode);
	free(dentry);

	return 0;
}

const struct inode_operations romfs_iops = {
	.lookup = romfs_lookup,
};

const struct file_operations romfs_fops = {
	.open = romfs_open,
	.read = romfs_read,
	.mmap = romfs_mmap,
};

const struct dentry_operations romfs_dops = {
	.delete = romfs_delete,
};
