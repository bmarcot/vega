/*
 * drivers/clockevents/clockchar.c
 *
 * Copyright (c) 2019 Benoit Marcot
 */

#include <kernel/cdev.h>
#include <kernel/clockevents.h>
#include <kernel/device.h>
#include <kernel/errno-base.h>
#include <kernel/fs.h>
#include <kernel/ktime.h>
#include <kernel/mm.h>
#include <kernel/string.h>
#include <kernel/types.h>
#include <kernel/wait.h>

#include <uapi/kernel/clock.h>
#include <uapi/kernel/time.h>

#include <errno.h> //FIXME

struct clk_chr_evt {
	unsigned int		interrupt_count;
	struct list_head	wq;
	struct timespec		ts;
};

void clockchar_interrupt(struct clock_event_device *timer);
struct clock_event_device *clockevents_get_device2(dev_t devt);

static int clockchar_open(struct inode *inode, struct file *file)
{
	/* Allocate a clock_event struct */
	struct clk_chr_evt *evt = kmalloc(sizeof(*evt));
	if (!evt)
		return -ENOMEM;
	evt->interrupt_count = 0;
	INIT_LIST_HEAD(&evt->wq);

	/* Configure the clock event device */
	struct clock_event_device *dev = clockevents_get_device2(inode->i_cdev->dev);
	clockevent_set_event_handler(dev, clockchar_interrupt);
	clockevents_switch_state(dev, CLOCK_EVT_STATE_ONESHOT);
	dev->private_data = evt;

	file->f_private = dev;

	return 0;
}

static ssize_t clockchar_read(struct file *file, char *buf, size_t count,
			off_t offset)
{
	struct clock_event_device *dev = file->f_private;
	struct clk_chr_evt *evt = dev->private_data;

	int ret = wait_event_interruptible(&evt->wq, evt->interrupt_count);
	if (ret == -ERESTARTSYS) {
		errno = EINTR;
		return -1;
	}
	/* if (copy_to_user(buf, &evt->interrupt_count, sizeof(evt->interrupt_count))) { */
	/* 	errno = EFAULT; */
	/* 	return -1; */
	/* } */
	memcpy(buf, &evt->interrupt_count, sizeof(evt->interrupt_count));
	evt->interrupt_count = 0;

	return sizeof(evt->interrupt_count);
}

static long clockchar_ioctl(struct file *file, unsigned int cmd,
			unsigned long arg)
{
	struct clock_event_device *dev = file->f_private;
	struct clk_chr_evt *evt = dev->private_data;
	struct timespec ts;

	switch (cmd) {
	case CLOCK_RD_TIME:
		ts = ktime_to_timespec(clockevents_read_elapsed(dev));
		memcpy((void *)arg, &ts, sizeof(ts));
		break;

	case CLOCK_SET_TIME:
		memcpy(&evt->ts, (void *)arg, sizeof(evt->ts));
		break;

	case CLOCK_START:
		clockevents_program_event(dev, timespec_to_ktime(evt->ts));
		break;

	case CLOCK_STOP:
		clockevents_shutdown(dev);
		break;

	case CLOCK_PIE_ON:
		if (!(dev->features & CLOCK_EVT_FEAT_PERIODIC)) {
			pr_err("error: Clock device does not have a periodic interrupt");
			return -1;
		}
		clockevents_switch_state(dev, CLOCK_EVT_STATE_PERIODIC);
		break;

	case CLOCK_PIE_OFF:
		clockevents_switch_state(dev, CLOCK_EVT_STATE_ONESHOT);
		break;

	default:
		errno = ENOTTY;
		return -1;
	}

	return 0;
}

#define CLOCK_CHAR_MAJOR 3

static const struct file_operations clockchar_fops = {
	.open = clockchar_open,
	.read = clockchar_read,
	.ioctl = clockchar_ioctl,
};

void clockchar_init(void)
{
	register_chrdev(CLOCK_CHAR_MAJOR, 0, 0, "timer", &clockchar_fops);
}

int clockchar_dev_register(struct clock_event_device *dev, int minor)
{
	cdev_add(&dev->char_dev, MKDEV(CLOCK_CHAR_MAJOR, minor));

	return 0;
}

void clockchar_interrupt(struct clock_event_device *timer)
{
	struct clk_chr_evt *evt = timer->private_data;

	evt->interrupt_count++;
	wake_up(&evt->wq, 1);
}
