noinst_PROGRAMS = mkldr
mkldr_SOURCES = crt0-i386.S entryap.S main.cc elf32.cc lapic.cc mp.cc
mkldr_DEPENDENCIES = $(srcdir)/mkldr.lds
mkldr_LINK = $(LD) -o $@ $(LDFLAGS) -T$(srcdir)/mkldr.lds
mkldr_LDADD = -lsubc
EXTRA_DIST = mkldr.lds elf32.h
CLEANFILES =
