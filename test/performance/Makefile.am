noinst_PROGRAMS = \
TimeString

# TimeString
TimeString_LDFLAGS = \
$(CXXBINLINKFLAGS) \
-o TimeString$(EXEEXT)

TimeString_LDADD = \
-L$(top_builddir)/src/common -lopenwbem \
$(Z_LIBS) $(SSL_LIBS)\
$(BINLINKAPPENDFLAGS)

TimeString_SOURCES = \
TimeString.cpp


INCLUDES = \
-I$(top_srcdir)/src/ifcs \
-I$(top_srcdir)/src/common \
-I$(top_srcdir)/src/cim

