#ifndef LINUX_TYPES_H
#define LINUX_TYPES_H

typedef unsigned int   u32;
typedef int            s32;

typedef unsigned short u16;
typedef short          s16;

typedef unsigned char  u8;
typedef char           s8;

struct list_head {
    struct list_head *next, *prev;
};

#endif /* !LINUX_TYPES_H */