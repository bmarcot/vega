/*
 * include/kernel/exit.h
 *
 * Copyright (c) 2018 Benoit Marcot
 *
 */

#ifndef _KERNEL_EXIT_H
#define _KERNEL_EXIT_H

#define EXIT_SUCCESS	0
#define EXIT_FATAL	0x80
#define EXIT_CODE_MASK	0x7f

void do_exit(int status);

#endif /* !_KERNEL_EXIT_H */
