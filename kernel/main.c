/*
 * kernel/main.c
 *
 * Copyright (c) 2015-2017 Baruch Marcot
 */

#include <stdlib.h>
#include <sys/cdefs.h>

#include <kernel/fs.h>
#include <kernel/init.h>
#include <kernel/kernel.h>
#include <kernel/mm/page.h>
#include <kernel/mm/slab.h>
#include <kernel/sched.h>
#include <kernel/thread.h>

#include "platform.h"

extern char __early_stack_start__;
extern char __early_stack_end__;
extern char __text_start__;
extern char __text_end__;
extern char __rodata_start__;
extern char __rodata_end__;
extern char __data_start__;
extern char __data_end__;
extern char __bss_start__;
extern char __bss_end__;
extern char __pgmem_start__;
extern char __pgmem_end__;
extern char __pgmem_size__;
extern char __heap_start__;
extern char __heap_end__;
extern char __heap_size__;

int mtdchar_init(void);
int idle_init(void);
void __printk_init(void);
int minishell(void *options);
void memdev_init(void);
void kernel_heap_init(void *heap_start, size_t heap_size);
int open(const char *pathname, int flags);

/* <unistd.h> */
ssize_t read(int fd, void *buf, size_t count);
int close(int fd);

void print_version(void)
{
	char buf[] = {0, 0};
	int fd = open("/proc/version", 0);

	while (read(fd, &buf, 1))
		printk("%s", buf);
	close(fd);
	printk("\n");
}

__weak_symbol int main(__unused void *arg)
{
	print_version();
	minishell(NULL);

	return 0;
}

void print_linker_sections(void)
{
	printk("Memory map:\n");
	printk("  .text	  = %08x--%08x	%6d Bytes\n", &__text_start__,
		&__text_end__, &__text_end__ - &__text_start__);
	printk("  .rodata = %08x--%08x	%6d Bytes\n", &__rodata_start__,
		&__rodata_end__, &__rodata_end__ - &__rodata_start__);
	printk("  .data	  = %08x--%08x	%6d Bytes\n", &__data_start__,
		&__data_end__, &__data_end__ - &__data_start__);
	printk("  .bss	  = %08x--%08x	%6d Bytes\n", &__bss_start__,
		&__bss_end__, &__bss_end__ - &__bss_start__);
	printk("  .heap	  = %08x--%08x	%6d Bytes\n", &__heap_start__,
		&__heap_end__, &__heap_end__ - &__heap_start__);
	printk("  .pgmem  = %08x--%08x	%6d Bytes\n", &__pgmem_start__,
		&__pgmem_end__, &__pgmem_end__ - &__pgmem_start__);
}

struct thread_info *start_kernel(void)
{
	setup_arch();

	__printk_init();

	/* initialize the kernel's malloc */
	kernel_heap_init(&__heap_start__, (size_t) &__heap_size__);

	print_linker_sections();

	/* initialize the physical memory allocator */
	show_page_bitmap(); // init_pages();
	kmem_cache_init();

	sched_init();
	idle_init();

	struct task_struct *init_task = alloc_init_task();
	pr_info("Created init_task at <%p> with priority=%d", init_task,
		init_task->prio);

	/* Reclaim the early-stack physical memory.  In the current context, no
	 * page allocation after this point are allowed.    */
	pr_info("Reclaim early stack's physical memory (%d Bytes, order=%d).",
		&__early_stack_start__ - &__early_stack_end__,
		size_to_page_order(2048));
	free_pages((unsigned long)&__early_stack_end__, size_to_page_order(2048));

	procfs_init();
	devfs_init();
	memdev_init();
	mtdchar_init();

	/* do the platform-specific inits */
	__platform_init();

	printk("Kernel bootstrap done.\n--\n");

	return task_thread_info(init_task);
}
