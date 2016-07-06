#ifndef STDDEF_H
#define STDDEF_H

/* http://pubs.opengroup.org/onlinepubs/9699919799/basedefs/stddef.h.html */

#ifndef NULL
#define NULL  ((void*)0)
#endif

#ifdef __GNUC__
#  define offsetof(type, member)  __builtin_offsetof (type, member)
#else
#  define offsetof(type, member)  ((size_t)&((type *)0)->member)
#endif

/* Unsigned integer type of the result of the sizeof operator. */
#ifndef _SIZE_T
#define _SIZE_T
typedef unsigned int size_t;
#endif

#endif /* !STDDEF_H */
