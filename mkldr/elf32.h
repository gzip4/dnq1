#ifndef MKLDR_ELF32_H
#define MKLDR_ELF32_H

int check_elf_ident(const void *);
int load_elf_image(const void *);
long unsigned elf_entry(const void *);

#endif /* MKLDR_ELF32_H */
