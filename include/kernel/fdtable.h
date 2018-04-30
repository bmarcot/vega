/*
 * include/kernel/fdtable.h
 *
 * Copyright (c) 2018 Benoit Marcot
 */

#ifndef _KERNEL_FDTABLE_H
#define _KERNEL_FDTABLE_H

#define NR_OPEN_DEFAULT 8

struct file;

struct files_struct {
	unsigned long	fdtab;
	struct file	*fd_array[NR_OPEN_DEFAULT];
};

struct files_struct *alloc_files_struct(void);
void put_files_struct(struct files_struct *files);
struct file *alloc_file(void);
void put_file(struct file *file);
int fd_init(void);

#endif /* !_KERNEL_FDTABLE_H */
