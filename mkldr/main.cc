
extern "C" int printf(const char *, ...);


extern "C" void
loader_main(unsigned magic, unsigned mbi)
{
	printf("Hello: %p %p\n", magic, mbi);
}
