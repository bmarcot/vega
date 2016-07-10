#include <pthread.h>
#include <stddef.h>
#include <string.h>

int pthread_attr_setstacksize(pthread_attr_t *attr, size_t stacksize)
{
	if (attr == NULL)
		return -1;
	attr->stacksize = stacksize;

	return 0;
}

int pthread_attr_getstacksize(const pthread_attr_t *attr, size_t *stacksize)
{
	if (attr == NULL)
		return -1;
	*stacksize = attr->stacksize;

	return 0;
}

const pthread_attr_t pthread_attr_default = {
	.flags = 0,
	.stacksize = 1024
};

int pthread_attr_init(pthread_attr_t *attr)
{
	if (attr == NULL)
		return -1;
	memcpy(attr, &pthread_attr_default, sizeof(pthread_attr_t));

	return 0;
}
