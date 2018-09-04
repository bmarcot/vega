/*
 * kernel/fs/romfs.c
 *
 * Copyright (c) 2016-2018 Benoit Marcot
 */

/* https://www.kernel.org/doc/Documentation/filesystems/romfs.txt */

#include <libgen.h> /* for basename() */

#include <kernel/dcache.h>
#include <kernel/fdtable.h>
#include <kernel/fs.h>
#include <kernel/fs/romfs.h>
#include <kernel/fs/tmpfs.h>
#include <kernel/kernel.h>
#include <kernel/list.h>
#include <kernel/mm.h>
#include <kernel/stat.h>
#include <kernel/string.h>

#include <drivers/mtd/mtd.h>

const struct inode_operations romfs_iops;
const struct file_operations romfs_fops;
const struct dentry_operations romfs_dops;

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

const struct super_operations romfs_sops = {0};

// mount("/dev/mtd", "/media/flash", "romfs", 0, NULL);
int romfs_mount(const char *source, const char *target,
		const char *filesystemtype,
		unsigned long mountflags, const void *data)
{
	struct inode *source_in = D_INODE(__do_file_open(source, 0));
	if (!source_in)
		return -1;

	/* link mounted-over inode to parent directory */
	struct inode *target_in = make_dir(dev_inode(), basename((char *)target));
	if (!target_in)
		return -1;
	target_in->i_op = &romfs_iops;
	target_in->i_dentry->d_count = -1; /* sticky file */

	/* Allocate a super_block struct. Unmounting the filesystem will release
	 * the super_block. */
	struct super_block *super_block = kmalloc(sizeof(*super_block));
	if (!super_block) {
		kfree(target_in);
		return -1;
	}
	INIT_LIST_HEAD(&super_block->s_inodes);
	super_block->s_op = &romfs_sops;

	/* super_block is found at the begining of memory area on MTD dev */
	super_block->s_dev = source_in->i_rdev;
	super_block->s_iroot = target_in; //FIXME: super_block must point to dentry instead of inode
	target_in->i_sb = super_block;

	struct mtd_info *mtd = get_mtd_device(super_block->s_dev);
	target_in->i_private = (void *)offsetof_first_device_inode(ROMFS_SB(mtd));

	return 0;
}

static struct inode *
alloc_inode(struct romfs_inode *ri, struct super_block *sb, struct inode *dir, struct dentry *dentry)
{
	struct inode *inode = new_inode(sb);

	switch (be32_to_cpu(ri->next_filehdr) & ROMFS_FILETYPE_MASK) {
	case ROMFS_FILETYPE_DIR:
		inode->i_mode = S_IFDIR;
		inode->i_op = &romfs_iops;
		break;
	case ROMFS_FILETYPE_REG:
		inode->i_mode = S_IFREG;
		inode->i_fop = &romfs_fops;
		break;
	default:
		pr_err("File type not supported");
		return NULL;
	}
	inode->i_size = be32_to_cpu(ri->size);
	inode->i_sb = sb;
	d_instantiate(dentry, inode);
	//FIXME: inode has been added to super_block's list by calling to
	// make_dir() and creat_file(). We should build the inode from scratch
	// instead of calling those tmpfs functions.

	/* We store the offset to the on-device inode rather than the logical
	 * address of the on-device inode, because that does not work when the
	 * filesystem is stored in an external SPI flash device. */
	struct mtd_info *mtd = get_mtd_device(sb->s_dev);
	inode->i_private =
		(void *)offsetof_device_inode(ri, ROMFS_SB(mtd));

	return inode;
}

struct dentry *romfs_lookup(struct inode *dir, struct dentry *target)
{
	__u32 next_filehdr = 0;
	struct romfs_superblock *rs;
	struct romfs_inode *ri;

	/* get current on-device inode */
	struct mtd_info *mtd = get_mtd_device(dir->i_sb->s_dev);
	rs = ROMFS_SB(mtd);
	ri = ROMFS_I(rs, dir->i_private);

	/* enter and walk the directory */
	next_filehdr = align(be32_to_cpu(ri->spec_info), 16);
	ri = ROMFS_I(rs, next_filehdr);

	for (int i = 0; next_filehdr < rs->full_size; i++) {
		if (!strcmp(ri->file_name, target->d_name)) {
			struct inode *inode = alloc_inode(ri, dir->i_sb, dir, target);
			if (inode == NULL)
				return NULL;
			target->d_inode = inode;
			target->d_op = &romfs_dops;

			return target;
		}

		/* inspect next file in current directory */
		next_filehdr = align(be32_to_cpu(ri->next_filehdr), 16);
		if (!next_filehdr)
			break;
		ri = ROMFS_I(rs, next_filehdr);
	}

	return NULL;
}

ssize_t romfs_read(struct file *file, char *buf, size_t count, off_t offset)
{
	size_t retlen;
	size_t filesize = file->f_dentry->d_inode->i_size;
	struct inode *inode = file->f_dentry->d_inode;
	struct mtd_info *mtd = get_mtd_device(inode->i_sb->s_dev);
	struct romfs_superblock *rs = ROMFS_SB(mtd);
	struct romfs_inode *ri = ROMFS_I(rs, inode->i_private);
	int len = sizeof(struct romfs_inode)
		+ align_next(strlen(ri->file_name) + 1, 16);

	if (file->f_pos + count > filesize)
		count = filesize - offset;
	mtd_read(mtd, (off_t)inode->i_private + len + offset, count, &retlen,
		(unsigned char *)buf);

	return retlen;
}

int romfs_mmap(struct file *file, off_t offset, void **addr)
{
	size_t retlen;
	size_t filesize = file->f_dentry->d_inode->i_size;
	struct inode *inode = file->f_dentry->d_inode;
	struct mtd_info *mtd = get_mtd_device(inode->i_sb->s_dev);
	struct romfs_superblock *rs = ROMFS_SB(mtd);
	struct romfs_inode *ri = ROMFS_I(rs, inode->i_private);
	int len = sizeof(struct romfs_inode)
		+ align_next(strlen(ri->file_name) + 1, 16);

	return mtd_point(mtd,
			(off_t)inode->i_private + len + offset,	filesize,
			&retlen, addr);
}

int romfs_delete(struct dentry *dentry)
{
	/* release in-memory inode */
	/* the root inode is deleted on unmount(), operation is pointed
	 * by i_sb->s_op->unmount() */
	if (dentry->d_inode != dentry->d_inode->i_sb->s_iroot)
		kfree(dentry->d_inode);

	return 0;
}

const struct inode_operations romfs_iops = {
	.lookup = romfs_lookup,
	.create = tmpfs_create,
	.mkdir = tmpfs_mkdir,
};

const struct file_operations romfs_fops = {
	.read = romfs_read,
	.mmap = romfs_mmap,
};

const struct dentry_operations romfs_dops = {
	.delete = romfs_delete,
};
