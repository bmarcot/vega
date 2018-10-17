/*
 * include/kernel/printk.h
 *
 * Copyright (c) 1991, 1992 Linus Torvalds
 * Copyright (c) 2018 Benoit Marcot
 */

#ifndef _KERNEL_PRINTK_H
#define _KERNEL_PRINTK_H

#include <kernel/kern_levels.h>
#include <kernel/types.h>

#define CONFIG_MESSAGE_LOGLEVEL_DEFAULT LOGLEVEL_DEBUG
#define CONFIG_CONSOLE_LOGLEVEL_DEFAULT LOGLEVEL_DEBUG

#if CONFIG_CONSOLE_LOGLEVEL_DEFAULT > CONFIG_MESSAGE_LOGLEVEL_DEFAULT
#warning "Lower priority log printout won't show in console"
#endif

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
	if (buffer[0] == KERN_SOH_ASCII && buffer[1]) {
		switch (buffer[1]) {
		case '0' ... '7':
		case 'd':/* KERN_DEFAULT */
		case 'c':/* KERN_CONT */
			return buffer[1];
		}
	}

	return 0;
}

/* Human readable text of the record */
static char *log_text(const struct printk_log *msg)
{
	return (char *)msg + sizeof(struct printk_log);
}

/* printk's without a loglevel use this */
#define MESSAGE_LOGLEVEL_DEFAULT CONFIG_MESSAGE_LOGLEVEL_DEFAULT

/* We show everything that is MORE important than this */
#define CONSOLE_LOGLEVEL_DEFAULT CONFIG_CONSOLE_LOGLEVEL_DEFAULT

#ifndef pr_fmt
#define pr_fmt(fmt) fmt
#endif

/*
 * pr_XXX() should produce zero code unless LOGLEVEL_MIN is lower
 * than LOGLEVEL_XXX
 */

#define no_printk(fmt, ...)				\
	({						\
		if (0)					\
			printk(fmt, ##__VA_ARGS__);	\
		0;					\
	})

#if MESSAGE_LOGLEVEL_DEFAULT >= LOGLEVEL_EMERG
#define pr_emerg(fmt, ...) \
	printk(KERN_EMERG pr_fmt(fmt), ##__VA_ARGS__);
#else
#define pr_emerg(fmt, ...) \
	no_printk(KERN_EMERG pr_fmt(fmt), ##__VA_ARGS__);
#endif

#if MESSAGE_LOGLEVEL_DEFAULT >= LOGLEVEL_ALERT
#define pr_alert(fmt, ...) \
	printk(KERN_ALERT pr_fmt(fmt), ##__VA_ARGS__);
#else
#define pr_alert(fmt, ...) \
	no_printk(KERN_EMERG pr_fmt(fmt), ##__VA_ARGS__);
#endif

#if MESSAGE_LOGLEVEL_DEFAULT >= LOGLEVEL_ALERT
#define pr_crit(fmt, ...) \
	printk(KERN_CRIT pr_fmt(fmt), ##__VA_ARGS__);
#else
#define pr_crit(fmt, ...) \
	no_printk(KERN_EMERG pr_fmt(fmt), ##__VA_ARGS__);
#endif

#if MESSAGE_LOGLEVEL_DEFAULT >= LOGLEVEL_ERR
#define pr_err(fmt, ...) \
	printk(KERN_ERR pr_fmt(fmt), ##__VA_ARGS__);
#else
#define pr_err(fmt, ...) \
	no_printk(KERN_EMERG pr_fmt(fmt), ##__VA_ARGS__);
#endif

#if MESSAGE_LOGLEVEL_DEFAULT >= LOGLEVEL_WARNING
#define pr_warn(fmt, ...) \
	printk(KERN_WARNING pr_fmt(fmt), ##__VA_ARGS__);
#else
#define pr_warn(fmt, ...) \
	no_printk(KERN_EMERG pr_fmt(fmt), ##__VA_ARGS__);
#endif

#if MESSAGE_LOGLEVEL_DEFAULT >= LOGLEVEL_NOTICE
#define pr_notice(fmt, ...) \
	printk(KERN_NOTICE pr_fmt(fmt), ##__VA_ARGS__);
#else
#define pr_notice(fmt, ...) \
	no_printk(KERN_EMERG pr_fmt(fmt), ##__VA_ARGS__);
#endif

#if MESSAGE_LOGLEVEL_DEFAULT >= LOGLEVEL_INFO
#define pr_info(fmt, ...) \
	printk(KERN_INFO pr_fmt(fmt), ##__VA_ARGS__);
#else
#define pr_info(fmt, ...) \
	no_printk(KERN_EMERG pr_fmt(fmt), ##__VA_ARGS__);
#endif

#if MESSAGE_LOGLEVEL_DEFAULT >= LOGLEVEL_DEBUG
#define pr_debug(fmt, ...) \
	printk(KERN_DEBUG pr_fmt(fmt), ##__VA_ARGS__);
#else
#define pr_debug(fmt, ...) \
	no_printk(KERN_EMERG pr_fmt(fmt), ##__VA_ARGS__);
#endif

#endif /* !_KERNEL_PRINTK_H */
