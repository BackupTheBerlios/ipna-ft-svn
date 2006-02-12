bin_PROGRAMS=ipnaft-fanout
ipnaft_fanout_SOURCES=Logger.cc ipnaft.1 aclocal.m4

#  uncomment the following if ipnaft requires the math library
ipnaft_fanout_LDADD=-lboost_program_options -lQtCore -lQtNetwork

EXTRA_DIST=ipnaft.lsm.in #NAME.texinfo ipnaft.spec.in

#  if you write a self-test script named `chk', uncomment the
#  following and add `chk' to the EXTRA_DIST list
#TESTS=chk

#  build and install the .info pages
info_TEXINFOS = ipnaft.texinfo
ipnaft_TEXINFOS = gpl.texinfo

#  install the man pages
man_MANS=ipnaft.1
