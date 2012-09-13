noinst_PROGRAMS = mkldr
mkldr_SOURCES = crt0-i386.S main.cc elf32.cc lapic.cc mp.cc entryapbin.cc
mkldr_DEPENDENCIES = $(srcdir)/mkldr.lds
mkldr_LINK = $(LD) -o $@ $(LDFLAGS) -T$(srcdir)/mkldr.lds
mkldr_LDADD = -lsubc
EXTRA_DIST = mkldr.lds elf32.h
CLEANFILES = entryap.bin entryap.bin.tmp entryap.bin.asm

entryapbin.cc: entryap.bin
	cat $< | $(srcdir)/bin2c.pl entryap_code > $@

entryap.bin: entryap.o
	$(LD) -melf_i386 -e start_ap -Ttext 0x7000 -o $@.tmp $<
	objcopy -S -O binary -j .text $@.tmp $@
	objdump -d $@.tmp > $@.asm

entryap.o: $(srcdir)/entryap.S
	$(CC) -m32 -c -nostdinc -fno-pic -o $@ $<
