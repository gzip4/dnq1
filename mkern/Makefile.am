noinst_PROGRAMS = mkern
mkern_SOURCES = crt0-x86.S main.cc crtn.S
mkern_DEPENDENCIES = $(srcdir)/mkern.lds
mkern_LINK = $(LD) -o $@ $(LDFLAGS) -n -M -Map=mkern.map --cref -T$(srcdir)/mkern.lds
mkern_LDADD = -lsubc
EXTRA_DIST = mkern.lds
