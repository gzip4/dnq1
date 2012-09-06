noinst_PROGRAMS = mkldr
mkldr_SOURCES = crt0-i386.S main.cc
mkldr_DEPENDENCIES = $(srcdir)/mkldr.lds
mkldr_LINK = $(LD) -o $@ $(LDFLAGS) -T$(srcdir)/mkldr.lds
mkldr_LDADD = -lsubc
EXTRA_DIST = mkldr.lds

#lmgr_DEPENDENCIES = $(srcdir)/lmgr.lds $(srcdir)/elf32.h $(srcdir)/mbi.h
#lmgr_LDADD =
#lmgr_LINK = $(LD) -o $@ $(LDFLAGS) -T$(srcdir)/lmgr.lds

#EXTRA_DIST = lmgr.lds elf32.h mbi.h
