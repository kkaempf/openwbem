noinst_PROGRAMS = \
TimeString \
TimeFile \
Time_pthread_once \
TimeAtomicOps

# TimeString
TimeString_PROJECT_LIBS = \
-L$(top_builddir)/src/common -lopenwbem $(BLOCXX_LIBS)

TimeString_EXTERNAL_LIBS = \
$(Z_LIBS) $(SSL_LIBS)\
$(BINLINKAPPENDFLAGS)

TimeString_SOURCES = \
TimeString.cpp


# TimeFile
TimeFile_PROJECT_LIBS = \
-L$(top_builddir)/src/common -lopenwbem $(BLOCXX_LIBS)

TimeFile_EXTERNAL_LIBS = \
$(Z_LIBS) $(SSL_LIBS)\
$(BINLINKAPPENDFLAGS)

TimeFile_SOURCES = \
TimeFile.cpp


# Time_pthread_once
Time_pthread_once_EXTERNAL_LIBS = \
$(top_builddir)/src/common/libopenwbem.a \
$(Z_LIBS) $(SSL_LIBS) $(BLOCXX_LIBS) \
$(BINLINKAPPENDFLAGS)

Time_pthread_once_SOURCES = \
Time_pthread_once.cpp \
PerformanceCommon.hpp

# TimeAtomicOps
TimeAtomicOps_EXTERNAL_LIBS = \
$(top_builddir)/src/common/libopenwbem.a \
$(Z_LIBS) $(SSL_LIBS) $(BLOCXX_LIBS) \
$(BINLINKAPPENDFLAGS)

TimeAtomicOps_SOURCES = \
TimeAtomicOps.cpp \
PerformanceCommon.hpp


INCLUDES = \
-I$(top_srcdir)/src/ifcs \
-I$(top_srcdir)/src/common \
-I$(top_srcdir)/src/cim

include $(top_srcdir)/Makefile-executable.inc
