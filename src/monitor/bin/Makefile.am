monitorbin_PROGRAMS = owprivilegemonitor@OPENWBEM_LIBRARY_VERSION@
monitorbindir = $(libexecdir)/openwbem

owprivilegemonitor@OPENWBEM_LIBRARY_VERSION@_SOURCES = OW_PrivilegeMonitor.cpp

owprivilegemonitor@OPENWBEM_LIBRARY_VERSION@_LDADD = \
  -L$(top_builddir)/src/monitor -lowprivman \
  -L$(top_builddir)/src/common -lopenwbem $(BLOCXX_LIBS) \
  $(SSL_LIBS) $(ICONV_LIB) \
  $(BINLINKAPPENDFLAGS)

owprivilegemonitor@OPENWBEM_LIBRARY_VERSION@_LDFLAGS = $(CXXBINLINKFLAGS) -o owprivilegemonitor@OPENWBEM_LIBRARY_VERSION@$(EXEEXT)

AM_CPPFLAGS = \
-I$(top_srcdir)/src/common -I$(top_srcdir)/src/monitor \
-I$(top_srcdir)/src/ifcs

