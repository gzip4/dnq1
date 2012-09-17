#include <stdarg.h>
#include <multiboot.h>
#include <config.h>

#include <dnq/dnq.h>

#include "elf32.h"

extern "C" int printf(const char *, ...);

static void panic(const char *, ...);

static inline bool
mb_flag_p(const multiboot_info_t *mb, multiboot_uint32_t flag)
{
	return mb->flags & flag;
}

static void parse_cmdline(const char *s)
{
	//strlen(s);
}

template <class T1, class T2>
static inline void memcpy(T1 dst, T2 src, unsigned sz)
{
	uint8 *d = reinterpret_cast<uint8 *>(dst);
	uint8 *s = reinterpret_cast<uint8 *>(src);
	while (sz--) { *d++ = *s++; }
}


#if defined(CONFIG_SMP)
volatile bool cpu_wait = false;


static void mpenter()
{
	int sp;
	__asm volatile ("movl %%esp,%0" : "=r" (sp));
	printf("ready. esp=0x%08x\n", sp);

#if 0
	uint32 a, d;
	x86::rdmsr(MSR_IA32_APIC_BASE, &a, &d);
	printf("IA32_APIC_BASE: %08x-%08x\n", d, a);

	lapic_t<0xfee00000> apic;
	printf("LAPIC ID : %08x\n", apic.id());
	printf("LAPIC VER: %08x\n", apic.ver());
	printf("LAPIC LVT: %08x\n", apic.lvtn());

	printf("LAPIC SIV: %08x\n", apic.siv());
	apic.init();
	printf("LAPIC SIV: %08x\n", apic.siv());

	printf("AP\n");
#endif

	cpu_wait = false;
	for (;;) __asm volatile ("hlt");
}


extern int ncpu;
extern int ismp;
extern int ioapicid;
extern int *lapic;
extern void mpinit();
extern void lapicstartap(uint8 apicid, mword_t addr);
extern "C" void _start_ap();


static void boot_ap(uint8 apicid)
{
	static bool initialized = false;
	static const mword_t addr = 0x7000;
	static const mword_t stack = 0x8000;

	mword_t *code = (mword_t *) addr;
	if (!initialized) {
		memcpy(code, _start_ap, 200);
		code[-2] = (mword_t) mpenter;
		initialized = true;
	}
	code[-1] = stack + (apicid - 1) * 256;

	cpu_wait = true;

	printf("CPU#%d: ", apicid);
	lapicstartap(apicid, addr);
	while (cpu_wait) ;
}
#endif // defined(CONFIG_SMP)


extern "C" void
loader_main(multiboot_uint32_t magic, const multiboot_info_t *mb)
{
	if (magic != MULTIBOOT_BOOTLOADER_MAGIC) {
		panic("Not multiboot-compliant bootloader: 0x%08x", magic);
	}

	if ( ! mb ) {
		panic("Not multiboot-compliant bootloader: mbinfo is missing");
	}

	// XXX: nice hello string
	printf("Hello, World!\n");

	if (mb_flag_p(mb, MULTIBOOT_INFO_MEMORY)) {
		printf("Memory: %dK, %dK\n", mb->mem_lower, mb->mem_upper);
	}

	if (mb_flag_p(mb, MULTIBOOT_INFO_BOOTDEV)) {
		printf("Boot device: 0x%08x\n", mb->boot_device);
	}


	if (mb_flag_p(mb, MULTIBOOT_INFO_BOOT_LOADER_NAME)
		&& mb->boot_loader_name)
	{
		printf("Bootloader: %s\n", mb->boot_loader_name);
	} else {
		printf("Unknown bootloader\n");
	}

	if (mb_flag_p(mb, MULTIBOOT_INFO_CMDLINE) && mb->cmdline) {
		parse_cmdline((const char *) mb->cmdline);
	}

	// Show BIOS memory map
	if (mb_flag_p(mb, MULTIBOOT_INFO_MEM_MAP)) {
		printf("BIOS memory map:\n");
		uint32 mmap_addr = mb->mmap_addr;
		static const char *mtypes[] = { "unknown", "available", "reserved",
			"acpi reclaimable", "nvs", "bad" };
		while (mmap_addr < (mb->mmap_addr + mb->mmap_length)) {
			multiboot_memory_map_t *m = (multiboot_memory_map_t *) mmap_addr;
			uint32 start_low = (uint32) m->addr;
			uint32 end_low = (uint32) (m->addr + m->len - 1ULL);
			uint32 t_idx = (m->type >= 1) && (m->type <= 5) ? m->type : 0;
			printf("    %p-%p : %s\n", start_low, end_low, mtypes[t_idx]);
			mmap_addr += m->size + 4;
		}
	} else {
		panic("No BIOS memory map.");
	}



	uint32 entries[] = {0,0};

	// Load kernel and roottask
	if (mb_flag_p(mb, MULTIBOOT_INFO_MODS)) {
		printf("Modules: %d, @%p\n", mb->mods_count, mb->mods_addr);
		multiboot_module_t *m = (multiboot_module_t *) mb->mods_addr;
		if (mb->mods_count < 2) {
			panic("At least 2 modules expected: kernel, roottask");
		}
		static const char *mtype[] = {"kernel", "roottask", 0};
		for (uint32 i = 0; i < 2; ++i) {
			printf(" #%d %p-%p [%-8s]: %s\n", i, m[i].mod_start, m[i].mod_end,
				mtype[i], m[i].cmdline);
			void *pelf = (void *) m[i].mod_start;
			if (check_elf_ident(pelf)) {
				if ( ! load_elf_image(pelf) )
					panic("Cannot load module: %s", mtype[i]);
				entries[i] = elf_entry(pelf);
			} else {
				panic("ELF binary expected: %s", mtype[i]);
			}
		}
	} else {
		panic("No modules found.");
	}

#if defined(CONFIG_SMP)
	// XXX: ifdef CONFIG_SMP
	mpinit();
	printf("ncpu: %d, lapic: %p\n", ncpu, lapic);

	if (ncpu > NCPU)
		ncpu = NCPU;

	printf("CPU#0: ready\n");
	for (uint8 i = 1; i < ncpu; ++i)
	{
		boot_ap(i);
	}
#endif

	printf("---\n");
}


// panic impl
extern "C" void putc(int);
extern "C" int vsnprintf (char *, unsigned int, const char *, va_list);

static inline void print_string(const char *s)
{
	while ( *s )
		putc(*s++);
}

void panic(const char *fmt, ...)
{
	static char outbuf[256];
	va_list ap;
	int r;

	if ( fmt ) {
		va_start(ap, fmt);
		r = vsnprintf(outbuf, sizeof(outbuf), fmt, ap);
		va_end(ap);

		if ( r > 0 ) {
			print_string("PANIC: ");
			print_string(outbuf);
			print_string("\nHalt.\n");
		}
	}

	for ( ; ; ) ;

	for ( ; ; )
		__asm__ __volatile__ ("cli; hlt");
}
