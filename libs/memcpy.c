
typedef unsigned long size_t;

void *memcpy(void *dest, const void *src, size_t n)
{
	__asm__ __volatile__ (
		"rep; movsb"
		: /* no output */
		: "D"(dest), "S"(src), "c"(n)
		: "memory" );
	return dest;
}
