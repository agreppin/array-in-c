#include <assert.h>
#include <stdio.h>
#include "array.h"

int main(int ac, char *av[])
{
	int *a = array(int, 16);
	array_append(a, 15);
	for (short i = 0; i < 1024; ++i)
		array_append(a, i);
	array_append(a, 17);
	array_remove(a, 1);
	size_t cap = array_capacity(a);
	size_t len = array_length(a);
	printf("cap=%zu len=%zu\n", cap, len);
	printf(" 0: %d\n", a[0]);
	printf(" 1: %d\n", a[1]);
	printf("16: %d\n", a[16]);
	assert(a[16] == 15);
	array_free(a);
	return 0; (void)ac; (void)av;
}
