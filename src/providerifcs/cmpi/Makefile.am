SUBDIRS = common
include $(top_srcdir)/Makefile-shared-lib.inc
all-local : all-ow-shared-libs
clean-local : clean-ow-shared-libs

include $(top_srcdir)/Makefile-shared-lib-inst.inc
install-exec-local : install-ow-shared-libs
uninstall-local : uninstall-ow-shared-libs


noinst_LIBRARIES = libcmpiprovifc.a
OW_SHARED_LIBS = libcmpiprovifc.so
SHAREDLIBADD = \
-L$(top_builddir)/src/providerifcs/cmpi/common -lcmpisupport \
-L$(top_builddir)/src/wql/common -lowwqlcommon 

thelibdir = $(pkglibdir)/provifcs

#OW_CMPIPolledProviderProxy.cpp  \
#OW_CMPIIndicationProviderProxy.cpp \
#OW_CMPIMethodProviderProxy.cpp  

if DISABLE_ASSOCIATION_TRAVERSAL
ASSOCIATION_PROVIDER_S=
else
ASSOCIATION_PROVIDER_S= \
OW_CMPIAssociatorProviderProxy.cpp \
OW_CMPIAssociatorProviderProxy.hpp
endif

libcmpiprovifc_a_SOURCES = \
$(ASSOCIATION_PROVIDER_S) \
OW_CMPIInstanceProviderProxy.cpp \
OW_CMPIInstanceProviderProxy.hpp \
OW_CMPIMethodProviderProxy.hpp  \
OW_CMPIPolledProviderProxy.hpp  \
OW_CMPIIndicationProviderProxy.hpp \
OW_CMPIProviderIFC.cpp \
OW_CMPIProviderIFC.hpp \
OW_CMPIProviderIFCUtils.hpp \
OW_FTABLERef.hpp

#libOW_CppIFC_a_LDFLAGS = -L$(top_builddir)/src/common/
#libOW_CppIFC_a_LIBADD = -lOpenWBEM


INCLUDES = \
-I$(top_srcdir)/src/common \
-I$(top_srcdir)/src/ifcs \
-I$(top_srcdir)/src/cim \
-I$(top_srcdir)/src/cimom \
-I$(top_srcdir)/src/provider \
-I$(top_srcdir)/src/providerifcs/cmpi/common \
-I$(top_srcdir)/src/server \
-I$(top_srcdir)/src/wql/common

