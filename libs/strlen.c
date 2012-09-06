/* -*- mode: c++ -*- */

typedef unsigned long size_t;

size_t strlen(const char *s)
{
	size_t len = 0;
	while (*s++) { ++len; }
	return len;
}
