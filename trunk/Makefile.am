bin_PROGRAMS=ipnaft
ipnaft_SOURCES=ipnaft.cc ipnaft.1 xmalloc.c system.h aclocal.m4 getopt.c getopt1.c getopt.h

#  uncomment the following if ipnaft requires the math library
#ipnaft_LDADD=-lm

EXTRA_DIST=ipnaft.lsm.in #NAME.texinfo ipnaft.spec.in

#  if you write a self-test script named `chk', uncomment the
#  following and add `chk' to the EXTRA_DIST list
#TESTS=chk

#  build and install the .info pages
info_TEXINFOS = ipnaft.texinfo
ipnaft_TEXINFOS = gpl.texinfo

#  install the man pages
man_MANS=ipnaft.1
