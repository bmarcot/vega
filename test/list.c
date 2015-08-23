#include <stdio.h>

#include "linux/list.h"

struct kool_list {
	char *task_name;
	int i;
	struct list_head list;
};

/* test linked-list */

int test_list(void)
{
	struct kool_list *tmp, nodes[16];
	LIST_HEAD(alist);

	for (unsigned i = 0; i < 16; i++) {
		nodes[i].i = i;
		list_add(&nodes[i].list, &alist);
	}
	list_for_each_entry(tmp, &alist, list) {
		printf("%d... ", tmp->i);
	}
	printf("\n");

	return 0;
}
