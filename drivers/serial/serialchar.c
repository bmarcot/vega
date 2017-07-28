/*
 * drivers/serial/serialchar.c
 *
 * Copyright (c) 2016 Benoit Marcot
 */

#include <kernel/fs.h>
#include <kernel/sched.h>
#include <kernel/serial.h>
#include <kernel/types.h>

#include <asm/current.h>

void serialchar_callback(struct serial_info *serial)
{
	sched_enqueue(serial->owner);
	sched_elect(0);
}

int serialchar_open(struct inode *inode, struct file *file)
{
	file->f_private = inode->i_private;
	struct serial_info *serial = file->f_private;
	serial->owner = current;
	serial->callback = serialchar_callback;

	return 0;
}

ssize_t serialchar_read(struct file *file, char *buf, size_t count, off_t offset)
{
	(void)offset;

	size_t retlen;
	struct serial_info *serial = file->f_private;

	while (serial->rx_count < count) {
		sched_dequeue(current);
		sched_elect(0);
	}
	if (count == 1)
		return serial_getc(serial, buf);
	if (serial_gets(serial, count, &retlen, buf) < 0)
		return -1;

	return retlen;
}

ssize_t serialchar_write(struct file *file, const char *buf, size_t count, off_t *offset)
{
	(void)offset;

	size_t retlen;
	struct serial_info *serial = file->f_private;

	if (count == 1)
		return serial_putc(serial, *((char *)buf));
	if (serial_puts(serial, count, &retlen, buf) < 0)
		return -1;

	return retlen;
}

const struct file_operations serialchar_fops = {
	.open  = serialchar_open,
	.read  = serialchar_read,
	.write = serialchar_write,
};
