noinst_PROGRAMS = rtask
rtask_SOURCES = crt0-i386.S main.cc crtn-i386.S
rtask_DEPENDENCIES = $(srcdir)/rtask.lds
rtask_LDADD = -lsubc
rtask_LDFLAGS = -n -M -Map=rtask.map --cref
rtask_LINK = $(LD) -o $@ $(LDFLAGS) $(rtask_LDFLAGS) -T$(srcdir)/rtask.lds
EXTRA_DIST = rtask.lds
