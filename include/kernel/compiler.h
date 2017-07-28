/*
 * include/kernel/compiler.h
 *
 * Copyright (c) 2016 Benoit Marcot
 */

#ifndef _KERNEL_COMPILER_H
#define _KERNEL_COMPILER_H

/* GCC weak symbol declaration */
#ifndef __weak
#  define __weak __attribute__((weak))
#endif

#ifndef offsetof
#  define offsetof(type, member) __builtin_offsetof(type, member)
#endif

#endif /* !_KERNEL_COMPILER_H */
