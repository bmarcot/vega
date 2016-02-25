#include <stdio.h>

int sys_1_arg(int a)
{
	printf("sys 1 arg: %x\n", a);

	return 2;
}

int sys_2_arg(int a, int b)
{
	printf("sys 2 arg: %x %x\n", a, b);

	return 2;
}

int sys_3_arg(int a, int b, int c)
{
	printf("sys 3 arg: %x %x %x\n", a, b, c);

	return 2;
}

int sys_4_arg(int a, int b, int c, int d)
{
	printf("sys 4 arg: %x %x %x %x\n", a, b, c, d);

	return 2;
}
