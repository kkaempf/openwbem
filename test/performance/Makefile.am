noinst_PROGRAMS = \
TimeString \
TimeFile \
Time_pthread_once

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


# Time_pthread_once
Time_pthread_once_LDFLAGS = \
$(CXXBINLINKFLAGS) \
-o Time_pthread_once$(EXEEXT)

Time_pthread_once_LDADD = \
$(top_builddir)/src/common/libopenwbem.a \
$(Z_LIBS) $(SSL_LIBS)\
$(BINLINKAPPENDFLAGS)

Time_pthread_once_SOURCES = \
Time_pthread_once.cpp


INCLUDES = \
-I$(top_srcdir)/src/ifcs \
-I$(top_srcdir)/src/common \
-I$(top_srcdir)/src/cim

