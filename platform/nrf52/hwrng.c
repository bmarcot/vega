/*
 * platform/nrf52/hwrng.c
 *
 * Copyright (c) 2017 Benoit Marcot
 */

#include <sys/types.h>

#include <kernel/fs.h>
#include <kernel/irq.h>
#include <kernel/scheduler.h>
#include <kernel/thread.h>

#include "platform.h"

struct thread_info *owner;

static void hwrng_isr(void)
{
	sched_enqueue(owner);
	sched_elect(SCHED_OPT_NONE);
}

static ssize_t hwrng_read(struct file *file, char *buf, size_t count, off_t offset)
{
	(void)file, (void)offset;

	for (int i = 0; i < (int)count; i++) {
		NRF_RNG->TASKS_START = 1;
		if (!NRF_RNG->EVENTS_VALRDY) {
			CURRENT_THREAD_INFO(curr_thread);
			owner = curr_thread;
			sched_elect(SCHED_OPT_NONE);
			NRF_RNG->EVENTS_VALRDY = 0;
		}
		buf[i] = NRF_RNG->VALUE;
	}

	return count;
}

const struct file_operations hwrng_fops = {
	//.open  = hwrng_open, // FIXME: open once!
	.read  = hwrng_read,
};

static struct inode inode;

int nrf52_hwrng_init(void)
{
	struct dentry dentry = { .d_inode = &inode, .d_name  = "hwrng" };

	init_tmpfs_inode(&inode);
	inode.i_fop = &hwrng_fops;
	vfs_link(0, dev_inode(), &dentry);

	irq_attach(RNG_IRQn, hwrng_isr);
	NVIC_EnableIRQ(RNG_IRQn);

	NRF_RNG->CONFIG = RNG_CONFIG_DERCEN_Enabled << RNG_CONFIG_DERCEN_Pos;
	NRF_RNG->SHORTS =
		RNG_SHORTS_VALRDY_STOP_Enabled << RNG_SHORTS_VALRDY_STOP_Pos;
	NRF_RNG->INTENSET =
		RNG_INTENSET_VALRDY_Set << RNG_INTENSET_VALRDY_Pos;

	return 0;
}
