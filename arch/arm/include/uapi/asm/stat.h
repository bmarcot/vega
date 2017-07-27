/*
 * arch/arm/include/uapi/asm/stat.h
 *
 * Copyright (c) 2017 Baruch Marcot
 *
 */

#ifndef _ASM_STAT_H
#define _ASM_STAT_H

struct stat {
	unsigned long st_dev;
	unsigned long st_ino;
	/* unsigned long st_nlink; */
	unsigned int st_mode;
	unsigned int st_uid;
	unsigned int st_gid;
	/* unsigned int __pad0; */
	unsigned long st_rdev;
	unsigned long st_size;
	/* unsigned long st_atime; */
	/* unsigned long st_atime_nsec; */
	/* unsigned long st_mtime; */
	/* unsigned long st_mtime_nsec; */
	/* unsigned long st_ctime; */
	/* unsigned long st_ctime_nsec; */
	/* unsigned long st_blksize; */
	/* long st_blocks; */
	/* unsigned long __unused[3]; */
};

#endif /* !_ASM_STAT_H */
