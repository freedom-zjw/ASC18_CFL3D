#include <stdio.h>

int flag = 0;

void c_fhat_(int *n_)
{
	if (!flag)
	{
		printf("received n = %d\n", *n_);
		flag = 1;
	}
}
