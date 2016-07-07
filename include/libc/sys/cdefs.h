#ifndef SYS_CDEFS_H
#define SYS_CDEFS_H

/* GCC unused symbol declaration */
#ifndef __unused
#  define __unused __attribute__((unused))
#endif

/* GCC weak symbol declaration */
#ifndef __weak
#  define __weak   __attribute__((weak))
#endif

#endif /* !SYS_CDEFS_H */
