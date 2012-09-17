AC_INIT([dnq], [0.0.1], [gzip4ever@gmail.com])
AC_PREREQ([2.65])
AM_INIT_AUTOMAKE([foreign 1.10 dist-bzip2 no-define nostdinc])
AC_CONFIG_SRCDIR([mkern/main.cc])
AC_CONFIG_HEADERS([config.h])

: ${CFLAGS=""}
: ${CXXFLAGS=""}

# Checks for programs.
AC_PROG_CXX([clang++ g++ c++])
AC_PROG_CC([clang gcc cc])
AM_PROG_AS
AC_PROG_RANLIB
AM_CONDITIONAL([CLANG],[test x$CC = xclang])

# Features
AC_ARG_ENABLE([temps],
  [AS_HELP_STRING([--enable-temps], [enable gcc -save-temps (default: no)])])

AC_ARG_ENABLE([debug],
  [AS_HELP_STRING([--enable-debug], [enable debugging (default: no)])])

AC_ARG_WITH([ncpu],
  [AS_HELP_STRING([--with-ncpu=N], [max CPUs supported (default: 4)])],
  [],
  [with_ncpu=4])

AC_ARG_WITH([ntcb],
  [AS_HELP_STRING([--with-ntcb=N], [max TCBs supported (default: 1024)])],
  [],
  [with_ntcb=1024])


AS_IF([test x$with_ncpu = xyes], [with_ncpu=4])
AS_IF([test x$with_ncpu = xno], [with_ncpu=1])
AS_IF([test $with_ncpu != 1 -a $with_ncpu != 2 -a $with_ncpu != 4 -a $with_ncpu != 8 -a $with_ncpu != 16 ],
      [AC_MSG_FAILURE([--with-ncpu requires number (1|2|4|8|16)])])

AS_IF([test x$with_ntcb = xyes -o x$with_ntcb = xno ],
      [with_ntcb=1024])
AS_IF([test $with_ntcb -lt 32 ],
      [AC_MSG_FAILURE([--with-ntcb requires number >= 32])])

AC_DEFINE_UNQUOTED([NCPU],[$with_ncpu],[Max CPU number supported by kernel])
AC_DEFINE_UNQUOTED([NTCB],[$with_ntcb],[Max TCB number supported by kernel])

AS_IF([test $with_ncpu -gt 1],
  [AC_DEFINE([CONFIG_SMP],[1],[SMP kernel, NCPU>1])],
  [])

# Checks for libraries.

# Checks for header files.

dnl Find location of stdarg.h include file
AC_MSG_CHECKING([location of stdarg.h])
testdir=`$CC -print-file-name=include`
if test -f "$testdir/stdarg.h"; then
  STDARGDIR=$testdir
else
  STDARGDIR=""
fi

if test "x$STDARGDIR" != x; then
  AC_MSG_RESULT([$STDARGDIR])
else
  AC_MSG_FAILURE([stdarg.h not found])
fi

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.

CPPFLAGS="-I\$(top_srcdir)/include -I\$(top_builddir) -I$STDARGDIR $CPPFLAGS"

CFLAGS="-g -O2 -fno-builtin -nostdinc -Wall -W -Wconversion -Wstrict-aliasing=2 $CFLAGS"
CFLAGS="$CFLAGS -m32 -march=pentium -fomit-frame-pointer -mno-red-zone"
CFLAGS="$CFLAGS -mno-mmx -mno-sse -mno-sse2 -mno-sse3 -mno-ssse3 -mno-sse4"
if test "x$enable_temps" == "xyes"; then
  CFLAGS="$CFLAGS -save-temps"
else
  CFLAGS="$CFLAGS -pipe"
fi

CCASFLAGS="-g -m32 -march=pentium -D__ASM__ -fno-builtin -nostdinc $CCASFLAGS"

CXXFLAGS="$CFLAGS -fno-exceptions -fno-rtti $CXXFLAGS"

LDFLAGS="-melf_i386 -nostdlib -L\$(top_builddir)/libs $LDFLAGS"

AC_CONFIG_FILES([Makefile
                 libs/Makefile
                 mkern/Makefile
                 mkldr/Makefile
                 rtask/Makefile])
AC_OUTPUT
