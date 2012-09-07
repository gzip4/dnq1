#include "types.h"

void bzero(void *dst, size_t n)
{
	char *p = (char *)dst;
	if (p) while (n--) *p++ = 0;
}
