/*
 * kernel/hash.c
 *
 * Copyright (c) 2017 Baruch Marcot
 */

/* Credit to
 *   - http://www.cse.yorku.ca/~oz/hash.html
 *   - http://softwareengineering.stackexchange.com/q/49550
 */

#include <stddef.h>

unsigned long hash_djb2(unsigned char *str, size_t len)
{
	unsigned long hash = 5381;
	int c;

	for (int i = 0; i < (int)len; i++) {
		c = str[i];
		hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
	}

	return hash;
}

unsigned long hash_sdbm(unsigned char *str, size_t len)
{
	unsigned long hash = 0;
	int c;

	for (int i = 0; i < (int)len; i++) {
		c = str[i];
		hash = c + (hash << 6) + (hash << 16) - hash;
	}

	return hash;
}
