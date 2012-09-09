#include <stdarg.h>
#include <multiboot.h>

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

