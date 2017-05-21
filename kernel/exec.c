/*
 * kernel/exec.c
 *
 * Copyright (c) 2017 Baruch Marcot
 */

#include <kernel/elf.h>

int do_execve(const char *filename, char *const argv[], char *const envp[])
{
	(void)argv, (void)envp;

	elf_load_binary(filename);

	return 0;
}

int sys_execve(const char *filename, char *const argv[], char *const envp[])
{
	return do_execve(filename, argv, envp);
}
