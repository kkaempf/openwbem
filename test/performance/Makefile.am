noinst_PROGRAMS = \
TimeString \
TimeFile

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


# TimeFile
TimeFile_LDFLAGS = \
$(CXXBINLINKFLAGS) \
-o TimeFile$(EXEEXT)

TimeFile_LDADD = \
-L$(top_builddir)/src/common -lopenwbem \
$(Z_LIBS) $(SSL_LIBS)\
$(BINLINKAPPENDFLAGS)

TimeFile_SOURCES = \
TimeFile.cpp


INCLUDES = \
-I$(top_srcdir)/src/ifcs \
-I$(top_srcdir)/src/common \
-I$(top_srcdir)/src/cim

