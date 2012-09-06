AC_PREREQ([2.65])
AC_INIT([ftrop], [0.0.1], [])
AM_INIT_AUTOMAKE([ftrop], [0.0.1])
AC_CONFIG_SRCDIR([mkern/main.cc])
AC_CONFIG_HEADERS([config.h])

if test "x$CFLAGS" == x; then
  CFLAGS=""
fi
if test "x$CXXFLAGS" == x; then
  CXXFLAGS=""
fi

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AM_PROG_AS
AC_PROG_RANLIB

AC_MSG_CHECKING([version of gcc])
CC_VERSION=`echo __GNUC__ | $CC -E - | grep -v '\#'`
CC_SUBVERSION=`echo __GNUC_MINOR__ | $CC -E - | grep -v '\#'`
AC_MSG_RESULT([$CC_VERSION.$CC_SUBVERSION])


# Features
AC_ARG_ENABLE([temps],
  [AS_HELP_STRING([--enable-temps], [enable gcc -save-temps (default: no)])])

AC_ARG_ENABLE([debug],
  [AS_HELP_STRING([--enable-debug], [enable debugging (default: no)])])


# Checks for libraries.

dnl Find location of libgcc.a
AC_MSG_CHECKING([location of libgcc.a])
LIBGCC=`$CC $LIBGCCFLAGS -print-libgcc-file-name`
LIBGCCDIR=`echo $LIBGCC | sed 's,/[[^/]]*$,,'`
AC_MSG_RESULT([$LIBGCCDIR])

# Checks for header files.

dnl Find location of stdarg.h include file
AC_MSG_CHECKING([location of stdarg.h])
testdir=`echo $LIBGCC | sed 's,/[[^/]]*$,/include,'`
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

CPPFLAGS="-I\$(top_srcdir)/include -I$STDARGDIR $CPPFLAGS"
CFLAGS="-O2 -fno-builtin -nostdinc -Wall -W -Wconversion -Wstrict-aliasing=2 $CFLAGS"
CFLAGS="$CFLAGS -m32 -march=pentium -fomit-frame-pointer -mno-red-zone"
CFLAGS="$CFLAGS -mno-mmx -mno-sse -mno-sse2 -mno-sse3 -mno-ssse3 -mno-sse4"
if test "x$enable_temps" == "xyes"; then
  CFLAGS="$CFLAGS -save-temps"
else
  CFLAGS="$CFLAGS -pipe"
fi
CCASFLAGS="-m32 -march=pentium -D__ASM__ $CCASFLAGS"
CXXFLAGS="$CFLAGS -fno-exceptions -fno-rtti $CXXFLAGS"
LDFLAGS="-melf_i386 -nostdlib -L\$(top_builddir)/libs $LDFLAGS"

AC_CONFIG_FILES([Makefile
                 libs/Makefile
                 mkern/Makefile
                 mkldr/Makefile
                 rtask/Makefile])
AC_OUTPUT
