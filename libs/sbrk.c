
extern char _end[];
static char *sbrk_top = _end;

#define SBRK_MIN	4096

void *sbrk(int s)
{
	if (s > 0) {
		if (s < SBRK_MIN) s = SBRK_MIN;
		sbrk_top += SBRK_MIN;
		return sbrk_top;
	}
	if (s < 0) {
		return (void *) -1;
	}
	return sbrk_top;
}
