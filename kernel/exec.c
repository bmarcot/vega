/*
 * kernel/exec.c
 *
 * Copyright (c) 2017 Baruch Marcot
 */

#include <kernel/elf.h>
#include <kernel/syscalls.h>

SYSCALL_DEFINE(execve,
	const char	*filename,
	char		*const argv[],
	char		*const envp[])
{
	(void)argv, (void)envp;
	elf_load_binary(filename);

	return 0;
}
