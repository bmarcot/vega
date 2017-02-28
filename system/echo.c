/*
 * system/echo.c
 *
 * Copyright (c) 2017 Benoit Marcot
 */

#include <kernel/kernel.h>

int echo(int argc, char *argv[])
{
	if (argc == 1)
		return 0;
	for (int i = 1; i < argc; i++) {
		if (i == 1)
			printk("%s", argv[i]);
		else
			printk(" %s", argv[i]);
	}
	printk("\n");

	return 0;
}
