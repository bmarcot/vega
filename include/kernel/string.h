/*
 * include/kernel/string.h
 *
 * Copyright (c) 2018 Benoit Marcot
 */

#ifndef _KERNEL_STRING_H
#define _KERNEL_STRING_H

#include <string.h>

/* NewLib misses some forward declarations */
char *strsep(char **stringp, const char *delim);

#endif /* !_KERNEL_STRING_H */
