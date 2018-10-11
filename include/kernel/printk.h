/*
 * include/kernel/printk.h
 *
 * Copyright (c) 2018 Benoit Marcot
 */

#ifndef _KERNEL_PRINTK_H
#define _KERNEL_PRINTK_H

#define LOGLEVEL_DEFAULT 0
#define CONFIG_LOG_BUF_SHIFT 11

struct printk_log {
	u64	ts_nsec;	/* timestamp in nanoseconds */
	u16	len;		/* length of entire record */
	u16	text_len;	/* length of text buffer */
	u8	flags:5;	/* internal record flags */
	u8	level:3;	/* syslog level */
};

enum log_flags {
	LOG_NOCONS = 1,		/* already flushed, do not print to console */
	LOG_NEWLINE = 2,	/* text ended with a newline */
	LOG_PREFIX = 4,		/* text started with a prefix */
	LOG_CONT = 8,		/* text is a fragment of a continuation line */
};

static inline int printk_get_level(const char *buffer)
{
	return 0;
}

/* Human readable text of the record */
static char *log_text(const struct printk_log *msg)
{
	return (char *)msg + sizeof(struct printk_log);
}

#endif /* !_KERNEL_PRINTK_H */
