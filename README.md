# Cbind

A library to create and execute function invocations.

Key features:
 * Can wrap any function call with up to 16 arguments of any type
 * Wrapped function call can be called from any place including other threads
 * Flexible serialization and memory management

Supported platforms are Linux/x86_64 with gcc

Support for x86/arm/arm64, FreeBSD/MacOS and clang is in progress.

## Example:
```
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cbind.h"
static void *
alloc_cb(void *data, int size)
{
	void *res = malloc(size);
	memcpy(res, data, size);
	return res;
}

struct val {
	long s1;
	char s2;
	double s3;
};

void test(int a1, double a2, struct val a3, char a4)
{
	fprintf(stderr, "%i - %lf - %li - %c - %lf - %c\n",
		a1, a2, a3.s1, a3.s2, a3.s3, a4);
}

int
main(int argc, char **argv)
{
	(void)argc;
	(void)argv;

	struct val val = {100, 'c', 0.5};
	void *p = make_call(alloc_cb, test, 1, 0.2, val, '*');
	exec_call(p);
	free(p);
	return 0;
}
```
