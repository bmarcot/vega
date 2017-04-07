/*
 * include/kernel/fs/romfs.h
 *
 * Copyright (c) 2017 Benoit Marcot
 */

#ifndef _KERNEL_FS_ROMFS_H
#define _KERNEL_FS_ROMFS_H

#include <kernel/types.h>

#include <drivers/mtd/mtd.h>

struct romfs_superblock {
	__u8  magic_number[8];
	__u32 full_size;
	__u32 checksum;
	char  volume_name[0];
};

struct romfs_inode {
	__u32 next_filehdr;
	__u32 spec_info;
	__u32 size;
	__u32 checksum;
	char  file_name[0];
};

#define ROMFS_SUPER_BLOCK(sb) ({			\
	struct mtd_info *__mtd = (sb)->s_private;	\
	struct romfs_superblock *__rs = __mtd->priv;	\
	__rs;						\
})

#define ROMFS_INODE(rs, offset) ({			\
	__u32 __addr = (__u32)(rs) + (__u32)(offset);	\
	struct romfs_inode *__ri = (struct romfs_inode *)__addr; \
	__ri;						\
})

int romfs_mount(const char *source, const char *target,
		const char *filesystemtype,
		unsigned long mountflags, const void *data);

#endif /* !_KERNEL_FS_ROMFS_H */
