#ifndef STDDEF_H
#define STDDEF_H

/* http://pubs.opengroup.org/onlinepubs/9699919799/basedefs/stddef.h.html */

#define NULL  ((void*)0)

#ifdef __GNUC__
#  define offsetof(type, member)  __builtin_offsetof (type, member)
#else
#  define offsetof(type, member)  ((size_t)&((type *)0)->member)
#endif

/* Unsigned integer type of the result of the sizeof operator. */
typedef unsigned int size_t;

#endif /* !STDDEF_H */
