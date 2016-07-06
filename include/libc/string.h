/* http://pubs.opengroup.org/onlinepubs/009696699/basedefs/string.h.html */

#ifndef STRING_H
#define STRING_H

#include <stddef.h>

char *strcpy(char *dest, const char *src);
char *strncpy(char *dest, const char *src, size_t count);
char *strcat(char *dest, const char *src);
int strcmp(const char *cs, const char *ct);
int strncmp(const char *cs, const char *ct, size_t count);
size_t strlen(const char *s);
void *memset(void *s, int c, size_t count);
void *memcpy(void *dest, const void *src, size_t count);
int memcmp(const void *cs, const void *ct, size_t count);

#endif /* !STRING_H */
