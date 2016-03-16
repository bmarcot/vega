#include "bitmap.h"
#include "linux/types.h"

unsigned bitmap_count(u32 *bm, unsigned len)
{
        unsigned count = 0;

        for (unsigned i = 0; i < len; i++)
                count += bitmap_get(bm, i);

        return count;
}
