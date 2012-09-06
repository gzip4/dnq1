
typedef unsigned long size_t;

void *memset(void *s, int c, size_t n)
{
	unsigned char *p = (unsigned char *) s;
	while (n--) {
		*p++ = (unsigned char) c;
	}
	return s;
}
