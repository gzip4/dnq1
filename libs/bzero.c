/* -*- mode: c++ -*- */

typedef unsigned long size_t;

void bzero(void *dst, size_t n)
{
	char *p = (char *)dst;
	if (p) while (n--) *p++ = 0;
}
