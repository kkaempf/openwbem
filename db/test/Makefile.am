check_SCRIPTS = run.test
noinst_PROGRAMS = dbtest
dbtest_SOURCES = dbtest.cpp

dbtest_LDADD = \
-L$(top_builddir)/db/db -lowdb \
$(BINLINKAPPENDFLAGS)

dbtest_LDFLAGS = \
$(CXXBINLINKFLAGS) \
-lm \
-o dbtest$(EXEEXT)

INCLUDES = -I$(top_srcdir)/db/include -I$(top_srcdir)/src/common

TESTS = run.test

EXTRA_DIST = run.test test_words
