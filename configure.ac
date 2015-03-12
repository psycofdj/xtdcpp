m4_include([.build/aclocal.m4])

AC_PREREQ([2.61])
AC_INIT([xtd], [1.0])
AC_CONFIG_SRCDIR([common/src/types.hh])
AC_CONFIG_AUX_DIR(.build)
AC_CONFIG_MACRO_DIR([.build])
AM_INIT_AUTOMAKE([foreign subdir-objects])
AM_CONFIG_HEADER(config.h)
AM_SILENT_RULES([yes])
AC_PROG_RANLIB
XTD_PACKAGE(xtd)

CXXFLAGS="${CXXFLAGS} -g"
CLAGS="${CXXFLAGS} -g"

AC_OUTPUT(
	Makefile
	common/Makefile
  network/Makefile
  counters/Makefile
  serializer/Makefile
  servers/Makefile
  )
