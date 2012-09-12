noinst_PROGRAMS = mkern mkern.sym mkern.x mkern.s
mkern_SOURCES = crt0-x86.S main.cc trap.cc trapasm.S vectors.S
mkern_DEPENDENCIES = $(srcdir)/mkern.lds
mkern_LINK = $(LD) -o $@ $(LDFLAGS) -n -M -Map=mkern.map --cref -T$(srcdir)/mkern.lds
mkern_LDADD = -lsubc
EXTRA_DIST = mkern.lds
CLEANFILES = mkern.map vectors.S

vectors.S: Makefile
	$(srcdir)/vectors.pl >$@

mkern.sym: mkern Makefile
	nm mkern | sort >$@

mkern.x: mkern Makefile
	objdump -x mkern >$@

mkern.s: mkern Makefile
	objdump -D mkern >$@
