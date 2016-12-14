/*
 * drivers/serial/serialchar.c
 *
 * Copyright (c) 2016 Benoit Marcot
 */

#include <sys/types.h>

#include <kernel/device.h>
#include <kernel/fs/vnode.h>
#include <kernel/scheduler.h>
#include <kernel/serial.h>
#include <kernel/thread.h>

void serialchar_callback(struct serial_info *serial)
{
	sched_enqueue(serial->owner);
	sched_elect(0);
}

int serialchar_open(struct vnode *vn, int flags)
{
	(void)vn, (void)flags;

	struct device *dev = vn->v_data;
	struct serial_info *serial = dev->drvdata;

	serial->callback = serialchar_callback;

	CURRENT_THREAD_INFO(cur_thread);
	serial->owner = cur_thread;

	return 0;
}

int serialchar_read(struct vnode *vn, void *buf, size_t count, off_t off, size_t *n)
{
	(void)off;

	struct device *dev = vn->v_data;
	struct serial_info *serial = dev->drvdata;

	while (serial->rx_count < count) {
		CURRENT_THREAD_INFO(cur_thread);
		sched_dequeue(cur_thread);
		sched_elect(0);
	}

	if (count == 1)
		return serial_getc(serial, buf);

	return serial_gets(serial, count, n, buf);
}

int serialchar_write(struct vnode *vn, void *buf, size_t count, off_t off, size_t *n)
{
	(void)off;

	struct device *dev = vn->v_data;
	struct serial_info *serial = dev->drvdata;

	if (count == 1)
		return serial_putc(serial, *((char *)buf));

	return serial_puts(serial, count, n, buf);
}

static const struct vnodeops serialchar_vops = {
	.vop_open = serialchar_open,
	.vop_read = serialchar_read,
	.vop_write = serialchar_write,
};

struct cdev serialchar_cdev = {
	.cdev_vops = &serialchar_vops,
};
