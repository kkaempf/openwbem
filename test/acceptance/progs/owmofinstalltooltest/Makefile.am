noinst_PROGRAMS= \
owmofinstalltooltest

INCLUDES= \
-I$(top_srcdir)/test/acceptance/progs/common \
-I$(top_srcdir)/src/common \
-I$(top_srcdir)/src/cim

owmofinstalltooltest_LDADD = \
-L$(top_srcdir)/test/acceptance/progs/common -low_test_common \
-L$(top_builddir)/src/common -lopenwbem \
$(SSL_LIBS) $(Z_LIBS) \
$(BINLINKAPPENDFLAGS)

owmofinstalltooltest_LDFLAGS = \
$(CXXBINLINKFLAGS) \
-o owmofinstalltooltest$(EXEEXT)

owmofinstalltooltest_SOURCES = \
owmofinstalltooltest.cpp

