#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)
AC_INIT(FULL-PACKAGE-NAME, VERSION, BUG-REPORT-ADDRESS)
AC_CONFIG_SRCDIR([src/])
AC_CONFIG_HEADER([config.h])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
#AC_PROG_INSTALL

# Checks for libraries.
# FIXME: Replace `main' with a function in `-lboost_program_options':
AC_CHECK_LIB([boost_program_options], [main])

# Checks for header files.
AC_CHECK_HEADERS([arpa/inet.h netdb.h netinet/in.h sys/socket.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_CONST
AC_TYPE_SIZE_T

# Checks for library functions.
AC_HEADER_STDC
AC_CHECK_FUNCS([gethostbyname memset socket])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
