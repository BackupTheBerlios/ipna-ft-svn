dnl Process this file with autoconf to produce a configure script.

AC_INIT(src/fanout/main.cc)
dnl Every other copy of the package version number gets its value from here
AM_INIT_AUTOMAKE(ipnaft, 0.1.0)

dnl create a config.h file (Automake will add -DHAVE_CONFIG_H)
AM_CONFIG_HEADER(config.h)

AC_SUBST(VERSION)

ISODATE=`date +%Y-%m-%d`
AC_SUBST(ISODATE)

AC_CANONICAL_HOST

dnl Checks for programs.
AC_PROG_INSTALL
AC_PROG_CC
AC_PROG_CXX

dnl Checks for libraries.
AC_CHECK_LIB([boost_program_options], [main])
AC_CHECK_LIB([QtCore], [main])
AC_CHECK_LIB([QtNetwork], [main])

dnl Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS(unistd.h sys/param.h sys/time.h time.h sys/mkdev.h sys/sysmacros.h string.h memory.h fcntl.h dirent.h sys/ndir.h ndir.h alloca.h locale.h )
AC_CHECK_HEADERS([arpa/inet.h netdb.h netinet/in.h sys/socket.h unistd.h])
AC_CHECK_HEADERS([boost/shared_ptr.hpp boost/shared_array.hpp boost/program_options.hpp])
AC_CHECK_HEADERS([qt4/QtNetwork/QHostAddress qt/QtNetwork/QUdpSocket])

jm_CHECK_TYPE_STRUCT_UTIMBUF
AC_HEADER_MAJOR
AC_FUNC_ALLOCA
AC_STRUCT_TM
AC_STRUCT_ST_BLOCKS
AC_FUNC_CLOSEDIR_VOID
AC_CHECK_FUNCS(mkfifo)
AC_CHECK_FUNC(mknod)

dnl Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_CONST
AC_TYPE_SIZE_T

dnl Checks for library functions.
AC_HEADER_STDC
AC_CHECK_FUNCS([gethostbyname memset socket])

AC_OUTPUT(Makefile ipnaft.lsm ipnaft.spec)
