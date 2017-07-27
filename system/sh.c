/*
 * system/sh.c
 *
 * Copyright (c) 2016-2017 Benoit Marcot
 */

#include <string.h>
#include <unistd.h>

#include <kernel/kernel.h>
#include <kernel/types.h>

#include <system/cat.h>
#include <system/echo.h>
#include <system/kmastat.h>
#include <system/ls.h>
#include <system/sh.h>

#include "platform.h"

/* forward declarations of buitins */
static int builtin_reboot(void);
static int builtin_halt(void);

//static const char ESC_SEQ_CURSOR_BACKWARD[] = "\033[D";
static const char ESC_SEQ_ERASE_LINE[]      = "\033[K";

static const char TERM_PROMPT[]        = "$ ";
static const char TERM_CRLF[]          = "\r\n";

struct cmd_info cmd_info[] = {
	DECL_CMD(cat,     cat),
	DECL_CMD(echo,    echo),
	DECL_CMD(exit,    builtin_halt),
	DECL_CMD(halt,    builtin_halt),
	DECL_CMD(kmastat, kmastat),
	DECL_CMD(ls,      ls),
	DECL_CMD(quit,    builtin_halt),
	DECL_CMD(reboot,  builtin_reboot),
};

static int builtin_reboot(void)
{
	printk("Requesting system reboot\n");
	NVIC_SystemReset();

	return -1; /* should not get there */
}

static int builtin_halt(void)
{
	__platform_halt();

	return -1; /* should not get there */
}

static int parse_command_line(char *buf, char *argv[])
{
	int buflen = strlen(buf);
	int argc = 1;

	if (!buflen)
		return 0;
	while (*buf == ' ')
		buf++;
	argv[0] = (char *)buf;
	for (int i = 0; i < buflen; i++) {
		if (buf[i] == ' ') {
			buf[i++] = '\0';
			while (buf[i] == ' ')
				i++;
			argv[argc++] = (char *)&buf[i];
		}
	}

	return argc;
}

static void exec_command(char *buf, int fd)
{
	int argc;
	char *argv[ARG_COUNT_MAX];

	argc = parse_command_line(buf, argv);
	for (unsigned i = 0; i < ARRAY_SIZE(cmd_info); i++) {
		if (!strncmp(cmd_info[i].cmd_name, argv[0], cmd_info[i].cmd_len)) {
			cmd_info[i].cmd_fun(argc, argv);
			return;
		}
	}

	/* command not found */
	write(fd, "command not found: ", STATIC_STRLEN("command not found: "));
	write(fd, buf, strlen(buf));
	write(fd, TERM_CRLF, sizeof(TERM_CRLF) - 1);
}

int sprintf(char *str, const char *format, ...);

static void cursor_backward(int n, int fd)
{
	char ebuf[8];

	if (n > 0) {
		sprintf(ebuf, "\033[%dD", n);
		write(fd, ebuf, strlen(ebuf));
	}
}

static int cur;
static int cur_eol;
static char buf_line[BUF_LINE_LEN];

static void readline(int fd)
{
	char c;

	read(fd, &c, 1);
	switch (c) {
	case ASCII_CARRIAGE_RETURN:
		write(fd, TERM_CRLF, sizeof(TERM_CRLF) - 1);
		if (cur_eol > 0) {
			exec_command(buf_line, fd);
			cur = 0; /* relative position to prompt's last char */
			cur_eol = 0;
		}
		write(fd, TERM_PROMPT, sizeof(TERM_PROMPT) - 1);
		break;
	case ASCII_BACKSPACE:
	case ASCII_DELETE: //XXX: Qemu sends DEL instead of BS
		if (cur > 0) {
			if (cur < cur_eol) {
				for (int i = cur; i <= cur_eol; i++)
					buf_line[i - 1] = buf_line[i];
			}
			buf_line[--cur_eol] = ASCII_NULL;
			cur--;
			cursor_backward(1, fd);
			write(fd, ESC_SEQ_ERASE_LINE,
				sizeof(ESC_SEQ_ERASE_LINE) - 1);
			write(fd, &buf_line[cur], strlen(&buf_line[cur]));
			cursor_backward(cur_eol - cur, fd);
		}
		break;
	case ' ' ... '~':
		if (cur < cur_eol) {
			for (int i = cur_eol; i >= cur; i--)
				buf_line[i + 1] = buf_line[i];
		}
		buf_line[cur++] = c;
		buf_line[++cur_eol] = '\0';
		write(fd, &buf_line[cur - 1], strlen(&buf_line[cur - 1]));
		cursor_backward(cur_eol - cur, fd);
		break;
	case ASCII_ESCAPE:
		read(fd, &c, 1);
		if (c != '[') // this is not an escape sequence
			return;
		read(fd, &c, 1);
		switch (c) {
		case 'C':
			if (cur < cur_eol) {
				cur++;
				write(fd, "\033[C", 3);
			}
			break;
		case 'D':
			if (cur > 0) {
				cur--;
				write(fd, "\033[D", 3);
			}
			break;
		default:
			printk("Unhandled escape sequence!\n");
			return;
		}
	default:
		;
	}
}

int minishell(void *options)
{
	(void)options;

	int fd = open("/dev/ttyS0", 0);
	if (fd < 0) {
		pr_err("Cannot open /dev/ttyS0");
		return -1;
	}

	write(fd, TERM_PROMPT, sizeof(TERM_PROMPT) - 1);
	for (;;) {
		readline(fd);
	}

	return 0;
}
