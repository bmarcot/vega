#ifndef BITMAP_H
#define BITMAP_H

#include "linux/types.h"

#define BITS_IN_U32 (sizeof (u32) * 8)

static inline void bitmap_set(u32 *bm, unsigned ix)
{
	bm[ix / BITS_IN_U32] |= (1 << (ix % BITS_IN_U32));
}

static inline void bitmap_clear(u32 *bm, unsigned ix)
{
	bm[ix / BITS_IN_U32] &= ~(1 << (ix % BITS_IN_U32));
}

static inline unsigned bitmap_get(u32 *bm, unsigned ix)
{
	return (bm[ix / BITS_IN_U32] >> (ix % BITS_IN_U32)) & 1;
}

unsigned bitmap_count(u32 *bm, unsigned len);

#endif /* !BITMAP_H */
