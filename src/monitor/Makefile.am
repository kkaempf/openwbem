if ENABLE_MONITORED_PERL_IFC
MPDIRS = Cifc Perlifc
else
MPDIRS =
endif

SUBDIRS = . bin $(MPDIRS) Test

YFLAGS = -t -v

MAKEDYLIB=1
include $(top_srcdir)/Makefile-shared-lib.inc
all-local : all-ow-shared-libs
clean-local : clean-ow-shared-libs

include $(top_srcdir)/Makefile-shared-lib-inst.inc
install-exec-local: install-ow-shared-libs
uninstall-local: uninstall-ow-shared-libs

OW_SHARED_LIBS = libowprivman.$(LIB_EXT)

libowprivman_PROJECT_LIBS = -L$(top_builddir)/src/common -lopenwbem
libowprivman_EXTERNAL_LIBS = $(BLOCXX_LIBS)
LIBRARY_BINARY_VERSION = $(OPENWBEM_LIBRARY_VERSION)
thelibdir = $(libdir)
owstatlibdir = $(thelibdir)

owstatlib_LIBRARIES = libowprivman.a

libowprivman_a_SOURCES = \
OW_PrivilegeLexer.ll \
OW_PrivilegeParser.yy \
OW_IPCIO.cpp \
OW_PrivilegeCommon.cpp \
OW_PrivilegeConfig.cpp \
OW_PrivilegeManager.cpp \
OW_PrivilegeManagerMockObject.cpp \
OW_PrivilegeManagerMockObjectScope.cpp \
OW_MonitoredPerl.cpp \
OW_IPCIO.hpp \
OW_PrivilegeCommon.hpp

# Some versions of flex generate sign comparison warnings, so ignore them.
libowprivman_a_CXXFLAGS = $(AM_CXXFLAGS) $(WNO_SIGN_COMPARE)

pkginclude_HEADERS = \
OW_PrivManOpenFlags.h \
OW_PrivilegeManager.hpp \
OW_PrivilegeManagerMockObject.hpp \
OW_PrivilegeManagerMockObjectScope.hpp \
OW_LoggerSpec.hpp \
OW_MonitoredPerl.hpp \
OW_PrivilegeConfig.hpp \
OW_PrivilegeLexer.hpp

EXTRA_DIST = \
$(top_builddir)/src/monitor/OW_PrivilegeLexer.cc \
$(top_builddir)/src/monitor/OW_PrivilegeParser.cc \
$(top_builddir)/src/monitor/OW_PrivilegeParser.h

# This is necessary in order for dependency tracking to work
BUILT_SOURCES = OW_PrivilegeParser.h

# For some strange reason automake doesn't insert this rule like it should...
OW_PrivilegeParser.h: OW_PrivilegeParser.cc
	@if test ! -f $@; then \
		rm -f OW_PrivilegeParser.cc; \
		$(MAKE) OW_PrivilegeParser.cc; \
	else :; fi


AM_CPPFLAGS = -I$(top_srcdir)/src/common

AM_LFLAGS = -Cfe -olex.yy.c

#debug versions
#AM_YFLAGS = -t -v
#AM_LFLAGS = -d -olex.yy.c
