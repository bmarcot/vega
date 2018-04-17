/*
 * kernel/exec.c
 *
 * Copyright (c) 2017-2018 Benoit Marcot
 */

#include <kernel/elf.h>
#include <kernel/syscalls.h>

SYSCALL_DEFINE(execve,
	const char	*filename,
	char		*const argv[],
	char		*const envp[])
{
	elf_exec_binary(filename);

	return 0;
}
