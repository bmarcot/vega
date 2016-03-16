#ifndef MM_H
#define MM_H

#include "linux/types.h"
#include "linux/list.h"

/* block size is at least the minimum page size of the machine */
#define MIN_PAGE_ORDER  8    /* min page size is 256 bytes */
#define MAX_BLOCK_ORDER 3    /* max block size is 2kB */

struct free_area {
	struct list_head free_list;
	u32 *map;
	int page_count;
};

int page_init(void);
void *page_alloc(int size);

#endif /* !MM_H */
