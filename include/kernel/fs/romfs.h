/*
 * include/kernel/fs/romfs.h
 *
 * Copyright (c) 2017 Benoit Marcot
 */

#ifndef _KERNEL_FS_ROMFS_H
#define _KERNEL_FS_ROMFS_H

struct romfs_superblock {
	u8   magic_number[8];
	u32  full_size;
	u32  checksum;
	char volume_name[0];
};

struct romfs_inode {
	u32  next_filehdr;
	u32  spec_info;
	u32  size;
	u32  checksum;
	char file_name[0];
};

#endif /* !_KERNEL_FS_ROMFS_H */
