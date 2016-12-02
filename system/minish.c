/*
 * system/minish.c
 *
 * Copyright (c) 2016 Benoit Marcot
 *
 * A minishell.
 */

#include <string.h>
#include <sys/types.h>

#include "kernel.h"

int open(const char *pathname, int flags);
ssize_t read(int fd, void *buf, size_t count);
ssize_t write(int fd, const void *buf, size_t count);

void ls(void);

int minishell(void *options)
{
	(void)options;

	int fd = open("/dev/ttyS0", 0);
	if (fd < 0)
		printk("cannot open /dev/ttyS0\n");

	char buf[64];
	int count = 0;

	const char SHELL_PROMPT[] = "$ ";
	const char NOT_FOUND[] = "command not found: ";
	const char CARRIAGE_RETURN[] = "\n\r";
	const char BUILTIN_HALT[] = "halt";
	const char BUILTIN_LS[] = "ls";
	const int NOT_FOUND_LEN = sizeof(NOT_FOUND);
	const int SHELL_PROMPT_LEN = sizeof(SHELL_PROMPT);

	const char BUILTIN_REBOOT[] = "reboot";
	const int BUILTIN_REBOOT_LEN = sizeof(BUILTIN_REBOOT);

	write(fd, SHELL_PROMPT, SHELL_PROMPT_LEN - 1);
	for (;;) {
		read(fd, &buf[count++], 1);
		if (buf[count - 1] == '\r') {
			write(fd, CARRIAGE_RETURN, 2);
			if (count == 1)
				goto null_cmd;
			if (!strncmp(BUILTIN_LS, buf, 2)) {
				ls();
			} else if (!strncmp(BUILTIN_REBOOT, buf,
						BUILTIN_REBOOT_LEN - 1)) {
				printk("Requesting system reboot\n");
				//NVIC_Reset();
			} else if (!strncmp(BUILTIN_HALT, buf, 4)) {
				semih_exit(0);
			} else {
				write(fd, NOT_FOUND, NOT_FOUND_LEN - 1);
				write(fd, buf, count - 1);
				write(fd, CARRIAGE_RETURN, 2);
			}
		null_cmd:
			count = 0;
			write(fd, SHELL_PROMPT, SHELL_PROMPT_LEN - 1);
		} else {
			write(fd, &buf[count - 1], 1);
		}
	}

	return 0;
}
