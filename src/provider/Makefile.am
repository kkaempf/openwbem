MAKEDYLIB=1
include $(top_srcdir)/Makefile-shared-lib.inc
all-local : all-ow-shared-libs
clean-local : clean-ow-shared-libs

include $(top_srcdir)/Makefile-shared-lib-inst.inc
install-exec-local : install-ow-shared-libs
uninstall-local : uninstall-ow-shared-libs

OW_SHARED_LIBS = libowprovider.$(LIB_EXT)
# increment this number every time a release breaks binary compatibility
LIBRARY_BINARY_VERSION = 4

owstatlib_LIBRARIES = libowprovider.a
SHAREDLIBADD = -L$(top_builddir)/src/common/ -lopenwbem

thelibdir = $(libdir)
owstatlibdir = $(thelibdir)

if DISABLE_ASSOCIATION_TRAVERSAL
ASSOCIATION_PROVIDER_S=
else
ASSOCIATION_PROVIDER_S= \
OW_AssociatorProviderIFC.cpp
endif

libowprovider_a_SOURCES = \
$(ASSOCIATION_PROVIDER_S) \
OW_IndicationExportProviderIFC.cpp \
OW_InstanceProviderIFC.cpp \
OW_SecondaryInstanceProviderIFC.cpp \
OW_MethodProviderIFC.cpp \
OW_PolledProviderIFC.cpp \
OW_ProviderBaseIFC.cpp \
OW_ProviderEnvironmentIFC.cpp \
OW_ProviderIFCBaseIFC.cpp \
OW_MethodProviderInfo.cpp \
OW_IndicationProviderIFC.cpp \
OW_ProviderInfoBase.cpp

if DISABLE_ASSOCIATION_TRAVERSAL
ASSOCIATION_PROVIDER_H=
else
ASSOCIATION_PROVIDER_H= \
OW_AssociatorProviderIFC.hpp \
OW_AssociatorProviderInfo.hpp
endif

pkginclude_HEADERS = \
$(ASSOCIATION_PROVIDER_H) \
OW_IndicationExportProviderIFC.hpp \
OW_InstanceProviderIFC.hpp \
OW_SecondaryInstanceProviderIFC.hpp \
OW_MethodProviderIFC.hpp \
OW_PolledProviderIFC.hpp \
OW_ProviderBaseIFC.hpp \
OW_ProviderEnvironmentIFC.hpp \
OW_ProviderIFCBaseIFC.hpp \
OW_InstanceProviderInfo.hpp \
OW_SecondaryInstanceProviderInfo.hpp \
OW_MethodProviderInfo.hpp \
OW_IndicationProviderIFC.hpp \
OW_IndicationProviderInfo.hpp \
OW_ProviderInfoBase.hpp \
OW_ProviderFwd.hpp

INCLUDES = \
-I$(top_srcdir)/src/common \
-I$(top_srcdir)/src/cim \
-I$(top_srcdir)/src/ifcs

