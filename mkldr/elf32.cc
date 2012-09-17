#include <dnq/types.h>
#include "elf32.h"
#include "subc.h"

struct eh32
{
  uint8  e_ident[16];
  uint16 e_type;
  uint16 e_machine;
  uint32 e_version;
  uint32 e_entry;
  uint32 e_phoff;
  uint32 e_shoff;
  uint32 e_flags;
  uint16 e_ehsz;
  uint16 e_phentsize;
  uint16 e_phnum;
  uint16 e_shentsize;
  uint16 e_shnum;
  uint16 e_shstrndx;
};

struct ph32 {
  uint32 p_type;
  uint32 p_offset;
  uint32 p_vaddr;
  uint32 p_paddr;
  uint32 p_filesz;
  uint32 p_memsz;
  uint32 p_flags;
  uint32 p_align;
};


int check_elf_ident(const void *p)
{
	if (!p) return 0;
	eh32 *elf = (eh32 *) p;
	bool is_elf = elf->e_ident[0] == 0x7f
		&& elf->e_ident[1] == 'E'
		&& elf->e_ident[2] == 'L'
		&& elf->e_ident[3] == 'F';
	return is_elf;
}

int load_elf_image(const void *p)
{
	eh32 *elf = (eh32 *) p;
	uint32 _elf = (uint32) elf;
	uint32 ph_addr = _elf + elf->e_phoff;
	ph32 *ph = (ph32 *) ph_addr;

	for (uint32 i = 0; i < elf->e_phnum; ++i) {
		if (ph[i].p_type == 1) {
			printf(" -> %p-%p\n", ph[i].p_paddr,
				ph[i].p_paddr + ph[i].p_memsz - 1);
			memcpy( (void *) ph[i].p_paddr,
				(void *) (_elf + ph[i].p_offset),
				ph[i].p_filesz);
		}
	}

	return 1;
}

long unsigned elf_entry(const void *p)
{
	eh32 *elf = (eh32 *) p;
	return elf->e_entry;
}
