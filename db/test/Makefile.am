check_SCRIPTS = run.test
noinst_PROGRAMS = dbtest
dbtest_SOURCES = dbtest.cpp
dbtest_PROJECT_LIBS = -L$(top_builddir)/db/db -lowdb

INCLUDES = -I$(top_srcdir)/db/include -I$(top_srcdir)/src/common

TESTS = run.test

EXTRA_DIST = run.test test_words

include $(top_srcdir)/Makefile-executable.inc
