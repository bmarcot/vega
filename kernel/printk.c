/*
 * kernel/printk.c
 *
 * Copyright (c) 1991, 1992 Linus Torvalds
 * Copyright (c) 2016-2018 Benoit Marcot
 */

#include <kernel/errno-base.h>
#include <kernel/ktime.h>
#include <kernel/list.h>
#include <kernel/mm.h>
#include <kernel/printk.h>
#include <kernel/sched_clock.h>
#include <kernel/stdarg.h>
#include <kernel/stdio.h>
#include <kernel/string.h>
#include <kernel/syscalls.h>
#include <kernel/syslog.h>
#include <kernel/types.h>
#include <kernel/wait.h>

int vsnprintf(char *str, size_t size, const char *format, va_list ap);
int snprintf(char *str, size_t size, const char *format, ...);
void __printk_putchar(char c);

extern int sched_clock_registered;

static LIST_HEAD(log_wait);

/* The next printk record to read by syslog(READ) or /proc/kmsg */
static u64 syslog_seq;
static u32 syslog_idx;
static size_t syslog_partial;

/* Index number of the first record stored in the buffer */
static u64 log_first_seq;
static u32 log_first_idx;

/* Index number of the next record to store in the buffer */
static u64 log_next_seq;
static u32 log_next_idx;

/* The next printk record to write to the console */
static u64 console_seq;
static u32 console_idx;

#define PREFIX_MAX 16
#define LOG_LINE_MAX 80

/* Record buffer */
#define LOG_ALIGN __alignof__(struct printk_log)
#define __LOG_BUF_LEN (1 << CONFIG_LOG_BUF_SHIFT)
static char __log_buf[__LOG_BUF_LEN] __aligned(LOG_ALIGN);
static char *log_buf = __log_buf;
static u32 log_buf_len = __LOG_BUF_LEN;

static size_t msg_print_text(const struct printk_log *msg, char *buf, size_t size);

static int console_print_log(struct printk_log *msg)
{
	int len;
	static char buf[LOG_LINE_MAX + PREFIX_MAX];

	len= msg_print_text(msg, buf, LOG_LINE_MAX + PREFIX_MAX);
	buf[len] = '\0';
	puts(buf);

	return 0;
}

/* Get record by index; idx must point to valid msg */
static struct printk_log *log_from_idx(u32 idx)
{
	struct printk_log *msg = (struct printk_log *)(log_buf + idx);

	/*
	 * A length == 0 record is the end of buffer marker. Wrap around and
	 * read the message at the start of the buffer.
	 */
	if (!msg->len)
		return (struct printk_log *)log_buf;

	return msg;
}

/* Get next record; idx must point to valid msg */
static u32 log_next(u32 idx)
{
	struct printk_log *msg = (struct printk_log *)(log_buf + idx);

	/*
	 * A length == 0 record is the end of buffer marker. Wrap around and
	 * read the message at the start of the buffer as *this* one, and
	 * return the one after that.
	 */
	if (!msg->len) {
		msg = (struct printk_log *)log_buf;
		return msg->len;
	}

	return idx + msg->len;
}

#define max(a, b) ((a) > (b) ? (a) : (b))

static int logbuf_has_space(u32 msg_size, bool empty)
{
	u32 free;

	if (log_next_idx > log_first_idx || empty)
		free = max(log_buf_len - log_next_idx, log_first_idx);
	else
		free = log_first_idx - log_next_idx;

	/*
	 * We need space also for an empty header that signalizes wrapping
	 * of the buffer.
	 */
	return free >= msg_size + sizeof(struct printk_log);
}

static int log_make_free_space(u32 msg_size)
{
	while ((log_first_seq < log_next_seq) &&
		!logbuf_has_space(msg_size, false)) {
		/* Drop old messages until we have enough contiguous space */
		log_first_idx = log_next(log_first_idx);
		log_first_seq++;
	}

	/* Sequence numbers are equal, so the log buffer is empty */
	if (logbuf_has_space(msg_size, log_first_seq == log_next_seq))
		return 0;

	return -ENOMEM;
}

/* Compute the message size including the padding bytes */
static u16 msg_used_size(u16 text_len, u16 *pad_len)
{
	u16 size;

	size = sizeof(struct printk_log) + text_len;
	*pad_len = (-size) & (LOG_ALIGN - 1);
	size += *pad_len;

	return size;
}

static int log_store(int level, enum log_flags flags, u64 ts_nsec,
		const char *text, u16 text_len)
{
	struct printk_log *msg;
	u16 size, pad_len;

	/* Number of '\0' padding bytes to next message */
	size = msg_used_size(text_len, &pad_len);

	/* Drop old messages to accomodate space for the new message */
	if (log_make_free_space(size))
		return -1;

	if (log_next_idx + size + sizeof(struct printk_log) > log_buf_len) {
		/*
		 * This message + an additional empty header does not fit at
		 * the end of the buffer. Add an empty header with len equal
		 * to 0 to signify a wrap around.
		 */
		memset(log_buf + log_next_idx, 0, sizeof(struct printk_log));
		log_next_idx = 0;
	}

	/* Fill message */
	msg = (struct printk_log *)(log_buf + log_next_idx);
	memcpy(log_text(msg), text, text_len);
	msg->text_len = text_len;
	msg->level = level & 7;
	msg->flags = flags & 0x1f;
	if (ts_nsec > 0)
		msg->ts_nsec = ts_nsec;
	else if (sched_clock_registered)
		msg->ts_nsec = sched_clock();
	memset(log_text(msg) + text_len, 0, pad_len);
	msg->len = size;

	/* Insert message */
	log_next_idx += msg->len;
	log_next_seq++;

	return msg->text_len;
}

static int log_output(int level, enum log_flags flags, const char *text,
		u16 text_len)
{
	int r;
	struct printk_log *msg;

	r = log_store(level, flags, 0, text, text_len);
	while (console_seq != log_next_seq) {
		msg = (struct printk_log *)(log_buf + console_idx);
		if (!msg->len) {
			console_idx = 0;
			msg = (struct printk_log *)log_buf;
		}
		if (msg->level <= CONSOLE_LOGLEVEL_DEFAULT)
			console_print_log(msg);
		console_idx = log_next(console_idx);
		console_seq++;
	}
	wake_up(&log_wait, 1);

	return r;
}

int vprintk(const char *format, va_list args)
{
	static char textbuf[LOG_LINE_MAX];
	char *text = textbuf;
	int text_len;
	enum log_flags flags = 0;
	int level = MESSAGE_LOGLEVEL_DEFAULT;

	text_len = vsnprintf(text, sizeof(textbuf), format, args);
	if (text_len < 0)
		return -1;

	/* Mark and strip a trailing newline */
	if (text_len && text[text_len-1] == '\n') {
		text_len--;
		flags |= LOG_NEWLINE;
	}

	/* Strip kernel syslog prefix and extract log level or control flags */
	int kern_level;
	if ((kern_level = printk_get_level(text)) != 0) {
		switch (kern_level) {
		case '0' ... '7':
			level = kern_level - '0';
			/* fallthrough */
		case 'd': /* KERN_DEFAULT */
			flags |= LOG_PREFIX;
			break;
		case 'c': /* KERN_CONT */
			flags |= LOG_CONT;
		}

		text_len -= 2;
		text += 2;
	}

	return log_output(level, flags, text, text_len);
}

int printk(const char *format, ...)
{
	va_list args;
	int r;

	va_start(args, format);
	r = vprintk(format, args);
	va_end(args);

	return r;
}

static size_t msg_print_text(const struct printk_log *msg, char *buf, size_t size)
{
	const char *text = log_text(msg);
	struct timespec ts;
	int prefix_len;

	ts = ktime_to_timespec(msg->ts_nsec);
	prefix_len = snprintf(buf, size, "[% 4d.%06d] ", (int)ts.tv_sec,
			(int)ts.tv_nsec / 1000);
	for (int i = 0; i < msg->text_len; i++)
		buf[prefix_len + i] = text[i];
	buf[prefix_len + msg->text_len] = '\n';

	return prefix_len + msg->text_len + 1;
}

static int syslog_print(char *buf, int size)
{
	char *text;
	struct printk_log *msg;
	int len = 0;

	text = kmalloc(LOG_LINE_MAX + PREFIX_MAX);
	if (!text)
		return -ENOMEM;

	while (size > 0) {
		size_t n;
		size_t skip;

		if (syslog_seq < log_first_seq) {
			/* Messages are gone, move to first one */
			syslog_seq = log_first_seq;
			syslog_idx = log_first_idx;
			syslog_partial = 0;
		}
		if (syslog_seq == log_next_seq)
			break;

		skip = syslog_partial;
		msg = log_from_idx(syslog_idx);
		n = msg_print_text(msg, text, LOG_LINE_MAX + PREFIX_MAX);
		if (n - syslog_partial <= (size_t)size) {
			/* Message fits into buffer, move forward */
			syslog_idx = log_next(syslog_idx);
			syslog_seq++;
			n -= syslog_partial;
			syslog_partial = 0;
		} else if (!len) {
			/* Partial read(), remember position */
			n = size;
			syslog_partial += n;
		} else {
			n = 0;
		}

		if (!n)
			break;

		//FIXME: copy_to_user(...)
		memcpy(buf, text + skip, n);

		len += n;
		size -= n;
		buf += n;
	}

	return len;
}

int do_syslog(int type, char *buf, int len)
{
	int error = 0;

	switch (type) {
	case SYSLOG_ACTION_READ:	/* Read from log */
		if (!buf || len < 0)
			return -EINVAL;
		if (!len)
			return 0;
		error = wait_event(&log_wait, syslog_seq != log_next_seq);
		if (error)
			return error;
		error = syslog_print(buf, len);
		break;
	default:
		error = -EINVAL;
		break;
	}

	return error;
}

SYSCALL_DEFINE(syslog,
	int		type,
	char		*buf,
	int		len)
{
	return do_syslog(type, buf, len);
}
