check_SCRIPTS = run.test
check_PROGRAMS = dbtest
dbtest_SOURCES = dbtest.c

dbtest_LDADD = \
-L$(top_builddir)/db/db -lowdb \
$(BINLINKAPPENDFLAGS)

dbtest_LDFLAGS = \
$(CBINLINKFLAGS) \
-lm

#-o dbtest

INCLUDES = -I$(top_srcdir)/db/include -I$(top_srcdir)/src/common

TESTS = run.test

EXTRA_DIST = run.test test_words
