monitorbin_PROGRAMS = owprivilegemonitor@OPENWBEM_LIBRARY_VERSION@
monitorbindir = $(libexecdir)/openwbem

owprivilegemonitor@OPENWBEM_LIBRARY_VERSION@_SOURCES = OW_PrivilegeMonitor.cpp


owprivilegemonitor@OPENWBEM_LIBRARY_VERSION@_PROJECT_LIBS = \
  -L$(top_builddir)/src/monitor -lowprivman \
  -L$(top_builddir)/src/common -lopenwbem

owprivilegemonitor@OPENWBEM_LIBRARY_VERSION@_EXTERNAL_LIBS = \
  $(BLOCXX_LIBS) \
  $(SSL_LIBS) $(ICONV_LIB)

AM_CPPFLAGS = \
-I$(top_srcdir)/src/common \
-I$(top_srcdir)/src/monitor \
-I$(top_srcdir)/src/ifcs


OW_EXECUTABLES = $(monitorbin_PROGRAMS)
include $(top_srcdir)/Makefile-executable.inc
