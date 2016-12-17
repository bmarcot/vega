/*
 * system/minish.c
 *
 * Copyright (c) 2016 Benoit Marcot
 */

#include <stdio.h>
#include <string.h>
#include <sys/types.h>

#include <kernel/kernel.h> //XXX: printk()

#include "minish.h"
#include "platform.h"

int open(const char *pathname, int flags);
ssize_t read(int fd, void *buf, size_t count);
ssize_t write(int fd, const void *buf, size_t count);
void ls(void);

static const char ESC_SEQ_CURSOR_BACKWARD[] = "\033[D";
static const char ESC_SEQ_ERASE_LINE[]      = "\033[K";

static const char TERM_PROMPT[]        = "$ ";
static const char TERM_CMD_NOT_FOUND[] = "command not found: ";
static const char TERM_CRLF[]          = "\r\n";

static const char BUILTIN_HALT[]   = "halt";
static const char BUILTIN_REBOOT[] = "reboot";
static const char BUILTIN_EXIT[]   = "exit";
static const char BUILTIN_LS[]     = "ls";

static void exec_command(const char *buf, int fd)
{
	if (!strncmp(BUILTIN_LS, buf, sizeof(BUILTIN_LS) - 1)) {
		ls();
	} else if (!strncmp(BUILTIN_REBOOT, buf, sizeof(BUILTIN_REBOOT))) {
		printk("Requesting system reboot\n");
		//NVIC_Reset(); // platform_reset();
	} else if (!strncmp(BUILTIN_HALT, buf, sizeof(BUILTIN_HALT))) {
		__platform_halt();
	} else if (!strncmp(BUILTIN_EXIT, buf, sizeof(BUILTIN_EXIT))) {
		__platform_halt();
	} else {
		write(fd, TERM_CMD_NOT_FOUND, sizeof(TERM_CMD_NOT_FOUND) - 1);
		write(fd, buf, strlen(buf));
		write(fd, TERM_CRLF, sizeof(TERM_CRLF) - 1);
	}
}

static int cur;
static int cur_eol;
static char buf_line[MINISH_LINE_MAX];

static void readline(int fd)
{
	char c;

	read(fd, &c, 1);
	switch (c) {
	case ASCII_CARRIAGE_RETURN:
		write(fd, TERM_CRLF, sizeof(TERM_CRLF) - 1);
		if (cur) {
			exec_command(buf_line, fd);
			cur = 0; /* relative position to prompt's last char */
			cur_eol = 0;
		}
		write(fd, TERM_PROMPT, sizeof(TERM_PROMPT) - 1);
		break;
	case ASCII_BACKSPACE:
	case ASCII_DELETE: //XXX: Qemu sends DEL instead of BS
		if (cur) {
			write(fd, ESC_SEQ_CURSOR_BACKWARD,
				sizeof(ESC_SEQ_CURSOR_BACKWARD) - 1);
			write(fd, ESC_SEQ_ERASE_LINE,
				sizeof(ESC_SEQ_ERASE_LINE) - 1);
			buf_line[--cur] = ASCII_NULL;
			cur_eol--;
		}
		break;
	case ' ' ... '~':
		if (cur < cur_eol) {
			for (int i = cur_eol; i >= cur; i--)
				buf_line[i + 1] = buf_line[i];
		}
		buf_line[cur++] = c;
		buf_line[++cur_eol] = '\0';
		char ebuf[8];
		write(fd, &buf_line[cur - 1], strlen(&buf_line[cur - 1]));
		if (cur < cur_eol) {
			sprintf(ebuf, "\033[%dD", cur_eol - cur);
			write(fd, ebuf, strlen(ebuf));
		}
		break;
	case ASCII_ESCAPE:
		read(fd, &c, 1);
		if (c != '[') // this is not an escape sequence
			return;
		read(fd, &c, 1);
		if (c != 'D') {
			printk("Unhandled escape sequence!\n");
			return;
		}
		cur--;
		write(fd, "\033[D", 3);
	default:
		;
	}
}

int minishell(void *options)
{
	(void)options;

	int fd = open("/dev/ttyS0", 0);
	if (fd < 0)
		printk("cannot open /dev/ttyS0\n");

	write(fd, TERM_PROMPT, sizeof(TERM_PROMPT) - 1);
	for (;;) {
		readline(fd);
	}

	return 0;
}
